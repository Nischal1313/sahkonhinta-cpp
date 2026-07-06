import json
from datetime import datetime
from urllib.request import urlopen

API_URL = "https://www.sahkohinta-api.fi/api/vartti/v1/halpa"
QUARTERS_PER_DAY = 96


def build_url(date: str | None = None) -> str:
    url = f"{API_URL}?vartit=96&tulos=haja"
    if date:
        url += f"&aikaraja={date}"
    return url


def fetch_prices(url: str) -> list[float]:
    with urlopen(url) as resp:
        data = json.loads(resp.read())
    if len(data) != QUARTERS_PER_DAY:
        raise ValueError(f"invalid API response size: {len(data)}")
    return [entry["hinta"] for entry in data]


def fetch_today() -> list[float]:
    return fetch_prices(build_url())


def fetch_for_date(date: str) -> list[float]:
    return fetch_prices(build_url(date))


def current_quarter_index() -> int:
    now = datetime.now()
    minutes_of_day = now.hour * 60 + now.minute
    return minutes_of_day // 15


def price_now(prices: list[float]) -> float:
    return prices[current_quarter_index()]


def main() -> None:
    today = fetch_today()
    past = fetch_for_date("2026-06-01")
    print(f"NOW PRICE: {price_now(today):.5f}")
    print(f"TODAY PRICE[0]: {today[0]:.5f}")
    print(f"PAST  PRICE[0]: {past[0]:.5f}")


if __name__ == "__main__":
    main()
