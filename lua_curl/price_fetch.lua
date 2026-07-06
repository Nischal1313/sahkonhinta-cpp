local M = {}

M.URL_TODAY = "https://api.spot-hinta.fi/Today"

local function parse_json_array(json_str)
    local results = {}
    -- Extract each object's PriceWithTax using a simple pattern match
    local pattern = '"PriceWithTax":%s*([%d%.%-]+)'
    local pos = 1
    while true do
        local start_i, end_i, val_str = json_str:find(pattern, pos)
        if not start_i then break end
        results[#results + 1] = tonumber(val_str)
        pos = end_i + 1
    end
    return results
end

function M.fetch_today()
    local cmd = 'curl -s --max-time 30 -H "Accept: application/json" "' .. M.URL_TODAY .. '"'
    local handle = io.popen(cmd)
    if not handle then
        print("ERROR: io.popen failed - is curl installed?")
        return nil
    end
    local body = handle:read("*a")
    local ok, exit_code = handle:close()
    if not ok then
        print("ERROR: curl exited with code", exit_code or "<unknown>")
        return nil
    end
    if not body or body == "" then
        print("ERROR: empty response")
        return nil
    end

    local prices = parse_json_array(body)
    if #prices == 0 then
        print("ERROR: no prices found in response")
        print("  Raw response (first 200 chars):", body:sub(1, 200))
        return nil
    end

    return prices
end

function M.pack_prices(prices)
    local parts = {}
    for _, price in ipairs(prices) do
        parts[#parts + 1] = string.pack("<f", price)
    end
    return table.concat(parts)
end

return M
