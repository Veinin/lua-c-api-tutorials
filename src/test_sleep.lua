require "msleep"

local start_time = os.time()
print(string.format("started at %d", start_time))

sleep(1)

local mid_time = os.time()
print(string.format("After sleep(1), time is %d", mid_time))

msleep(2000)

local end_time = os.time()
print(string.format("After sleep(1), time is %d", end_time))

print(string.format("First  interval = %d seconds", mid_time - start_time))
pritn(string.format("Second interval = %d seconds", end_time - mid_time))
print(string.format("Whole  interval = %d seconds", end_time - start_time))