# How to parse mein-abfallkalender.de data

## Get your town and street_id

* If your town is included most likely: $TOWN.mein-abfallkalender.de
* Curl the page and grep for your streetname, e.g.: `<option value="5882">Feldweg, Cappel</option>`
* The `option` is your street_id

## Parsing the HTML output

Best guess: There is only one `<table> ... </table>` part in the output and it contains what we want (and a bit more)

Storing the whole HTML for parsing is unlikely (15-18 kB HTML loaded on a 80kB RAM µc. No.)

So, Options:

1. Cut out table from HTML reply, parse it and store results

   * Con: This would mandate storing multiple dates locally
   * Con: So we need to get time to know which date it is
   * Pro: Only need network access once every month or so
   * Pro: Standalone solution

2. Modify request to get only the next bin to be picked up and its date

   * Con: Still requres NTP
   * Pro: Minimal version of 1.
   * Pro: Still standalone

3. Do all the parsing somewhere else and use the bindicator as a MQTT-subscriber which controlls the LED

   * Con: Smells even more like shitty IoT
   * Con: Multiple points of failure / "server" needed
   * Con: Far less useful for other people
   * Con: I'd be a lazy boi
   * Pro: Less code, no NTP needed (but, alas, see above)
   * Pro: I can also use it to indicate IRC mentions (or something useful)

Conclusion: I'll try 1 fist, 2 if 1 does not work out and afterwards try 3 to start up my IoT RPi-Rust-MQTT controlled home automation LED lighting system communicating messages in it's own light effect description language.... or, you know, not.

## Todo

* Get correct data from server
* Parse data correctly into struct
  * define that struct. Unixtime and color?
* Get time and keep it
* Check if currently awaited
* Put the next (several?) times and colors which need to be shown in the EEPROM (persist power drops and network outages!)
