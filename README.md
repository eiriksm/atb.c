# atb.c
C API for AtB schedule

[![Test](https://github.com/eiriksm/atb.c/actions/workflows/test.yml/badge.svg)](https://github.com/eiriksm/atb.c/actions/workflows/test.yml)

## About 

This project aims to provide a model and data for embedding AtB schedule data in something like a device without internet connection. In other words, the header file will contain schedules I need at the time of writing.

The data is therefore obviously not realtime in any way. They are simply downloaded and converted from here: https://developer.entur.org/stops-and-timetable-data (the GTFS format specifically). 

The data used is available under the license [NLOD](https://data.norge.no/nlod/en/2.0), and is made available by Entur on the link above.

This project is not affiliated with, or endorsed or approved by, Entur or AtB in any way.

## Schedule files

Instead of the schedules compiled into `atb.h`, schedules can be read from a
text file with `atb_file_count_entries`, `atb_file_get_entry` and
`atb_file_next_departures`. The file is read line by line through a callback,
so it never has to fit in memory. Define `ATB_NO_BUILTIN_SCHEDULES` to leave
the built in tables out entirely.

```
# Ugla, tram towards Sentrum
[UGL 9]
route   09_2
stop    71779
offset  5
mon-fri 05:57-18:12/15 18:42-23:42/30
sat     07:12-08:42/30 09:12-18:12/15 18:42-23:42/30
sun     09:12-23:42/30

[STO 9]
route   09_1
stop    74061
mon-fri 06:30-18:45/15 19:15-24:15/30
sat     07:45-09:45/30 10:00-18:45/15 19:15-24:15/30
sun     09:45-24:15/30
```

- Each `[LABEL ROUTE]` header starts a new entry. Entries are numbered from 0
  in file order. The label is cut to 3 characters and the route to 2, to fit
  the top of a Casio display.
- `#` starts a comment, and blank lines are ignored.
- `offset N` adds N minutes to the times on the lines after it, which is
  handy for listing times from the start of a route and adjusting for the
  stop. It resets to 0 for every entry.
- A day line is a day spec followed by times. Day specs are `mon` through
  `sun`, ranges like `mon-fri` or `fri-mon`, lists like `sat,sun`, or `daily`.
- A time is `HH:MM`, or `HH:MM-HH:MM/N` for every N minutes from the first to
  the last time, both included. Hours past 23, like `24:15`, are departures
  after midnight.
- Keep lines shorter than 120 characters. Repeat the day spec on a new line
  to continue a long list; all matching lines are combined.
- `route`, `stop` and anything else that is not a day spec are ignored.

Times are CET/CEST wall clock times, like the built in schedules.

## License 

MIT
