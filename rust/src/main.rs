use chrono::{Local, Timelike};
use serde::Deserialize;
use std::time::{SystemTime, UNIX_EPOCH};

const API_URL: &str = "https://www.sahkohinta-api.fi/api/vartti/v1/halpa";
const QUARTERS_PER_DAY: usize = 96;

#[derive(Deserialize)]
struct QuarterPrice {
    hinta: f64,
}

type QuarterDayPrices = [f64; QUARTERS_PER_DAY];

fn build_url(date: Option<&str>) -> String {
    let mut url = format!("{API_URL}?vartit=96&tulos=haja");
    if let Some(d) = date {
        url.push_str(&format!("&aikaraja={d}"));
    }
    url
}

fn fetch_prices(url: &str) -> Result<QuarterDayPrices, Box<dyn std::error::Error>> {
    let resp = reqwest::blocking::get(url)?.text()?;
    let data: Vec<QuarterPrice> = serde_json::from_str(&resp)?;
    if data.len() != QUARTERS_PER_DAY {
        return Err(format!("invalid API response size: {}", data.len()).into());
    }
    let mut prices = [0.0; QUARTERS_PER_DAY];
    for (i, qp) in data.iter().enumerate() {
        prices[i] = qp.hinta;
    }
    Ok(prices)
}

fn fetch_today() -> Result<QuarterDayPrices, Box<dyn std::error::Error>> {
    fetch_prices(&build_url(None))
}

fn fetch_for_date(date: &str) -> Result<QuarterDayPrices, Box<dyn std::error::Error>> {
    fetch_prices(&build_url(Some(date)))
}

fn current_quarter_index() -> usize {
    let now = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_secs();
    let dt = chrono::DateTime::from_timestamp(now as i64, 0)
        .unwrap()
        .with_timezone(&Local);
    let minutes_of_day = dt.hour() as u32 * 60 + dt.minute() as u32;
    (minutes_of_day / 15) as usize
}

fn price_now(prices: &QuarterDayPrices) -> f64 {
    prices[current_quarter_index()]
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let today = fetch_today()?;
    let past = fetch_for_date("2026-06-01")?;

    let now_price = price_now(&today);

    println!("NOW PRICE: {:.5}", now_price);
    println!("TODAY PRICE[0]: {:.5}", today[0]);
    println!("PAST  PRICE[0]: {:.5}", past[0]);

    Ok(())
}
