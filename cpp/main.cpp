#include <array>
#include <cstdint>
#include <ctime>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <iostream>
#include <iomanip>

using json = nlohmann::json;

constexpr std::size_t QUARTERS_PER_DAY = 96;

constexpr const char* API_URL =
    "https://www.sahkohinta-api.fi/api/vartti/v1/halpa";

using QuarterDayPrices = std::array<double, QUARTERS_PER_DAY>;

static size_t WriteCallback(
    void* contents,
    size_t size,
    size_t nmemb,
    void* userp)
{
    static_cast<std::string*>(userp)->append(
        static_cast<char*>(contents),
        size * nmemb);
    return size * nmemb;
}

static std::string HttpGet(const std::string& url)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        throw std::runtime_error("curl init failed");

    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
        throw std::runtime_error(curl_easy_strerror(res));

    return response;
}

static std::string BuildUrl(const std::string& date = "")
{
    std::string url =
        std::string(API_URL) +
        "?vartit=96&tulos=haja";

    if (!date.empty())
        url += "&aikaraja=" + date;

    return url;
}

static QuarterDayPrices ParsePrices(const std::string& response)
{
    json data = json::parse(response);

    if (data.size() != QUARTERS_PER_DAY)
        throw std::runtime_error("invalid API response size");

    QuarterDayPrices prices{};

    for (std::size_t i = 0; i < QUARTERS_PER_DAY; ++i)
        prices[i] = data[i]["hinta"].get<double>();

    return prices;
}

QuarterDayPrices FetchTodayPrices()
{
    return ParsePrices(HttpGet(BuildUrl()));
}

QuarterDayPrices FetchPricesForDate(const std::string& date)
{
    return ParsePrices(HttpGet(BuildUrl(date)));
}

static std::size_t CurrentQuarterIndex()
{
    std::time_t now = std::time(nullptr);
    std::tm local{};

#if defined(_WIN32)
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif

    uint8_t minutes_of_day =
        local.tm_hour * 60 + local.tm_min;

    return static_cast<std::size_t>(
        minutes_of_day / 15);
}


double GetCurrentPrice(const QuarterDayPrices& prices)
{
    return prices[CurrentQuarterIndex()];
}

double GetPriceNow(const QuarterDayPrices& prices)
{
    return prices[CurrentQuarterIndex()];
}

double GetPriceAtQuarter(
    const QuarterDayPrices& prices,
    std::size_t quarterIndex)
{
    if (quarterIndex >= prices.size())
        throw std::out_of_range("invalid quarter index");

    return prices[quarterIndex];
}

int main()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    try
    {
        auto today = FetchTodayPrices();
        auto past  = FetchPricesForDate("2026-06-01");

        double nowPrice = GetPriceNow(today);

        std::cout << std::fixed << std::setprecision(5);

        std::cout << "NOW PRICE: " << nowPrice << "\n";
        std::cout << "TODAY PRICE[0]: " << today[0] << "\n";
        std::cout << "PAST  PRICE[0]: " << past[0] << "\n";

        curl_global_cleanup();
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        curl_global_cleanup();
        return 1;
    }
}