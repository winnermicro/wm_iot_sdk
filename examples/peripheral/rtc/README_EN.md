# RTC Demo

## Function Overview

After starting this application, perform the following actions:
1. Initialize RTC.
2. Set RTC calendar.
3. Query RTC calendar and print the current time.
4. Register RTC alarm callback and enable RTC alarm clock.
5. In the RTC alarm callback function, loop start with RTC alarm clock.


## Building and Flashing

Example Location： `examples/peripheral/rtc`

For compilation, flashing, and other operations, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Result

The following log is displayed after successful operation

```
D/dt              [0.010] add device rtc
I/rtc_example     [0.014] now is: 2024-07-01 12:00:00
I/rtc_example     [4.912] alarm arrived: 2024-07-01 12:00:05
I/rtc_example     [9.904] alarm arrived: 2024-07-01 12:00:10
I/rtc_example     [14.898] alarm arrived: 2024-07-01 12:00:15
I/rtc_example     [19.892] alarm arrived: 2024-07-01 12:00:20
I/rtc_example     [24.888] alarm arrived: 2024-07-01 12:00:25
I/rtc_example     [29.880] alarm arrived: 2024-07-01 12:00:30
```
