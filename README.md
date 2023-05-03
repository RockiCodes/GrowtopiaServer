# Growtopia Server v1.0 ![Build Status](https://media.discordapp.net/attachments/1103007816471552050/1103007929449336836/68747470733a2f2f63692e6170707665796f722e636f6d2f6170692f70726f6a656374732f7374617475732f6769746875622f47726f77746f7069614e6f6f62732f47726f77746f706961536572766572.png)
Better/Faster Version of GrowtopiaNoobs's GrowtopiaServer Project. (**The First Growtopia Private Server with ENet**)

This project has been compiled with Visual Studio 2022 and 2019 or other compilers weren't been tested.

This project has been published under GNU AFFERO GPL license, so you need to **publish the whole source code and citate the orginal authors name, even if you are using your server as service**!

# **TODO list:**
- [ ] Refactor whole code, it is very hard readable and there might be problems with maintaining it
- [ ] Try to write multiple servers sharing only between themselves possibly world list, player list and broadcast queue
- [ ] Write event pool - this is needed to make delayed actions like respawning
- [ ] Make heavy events asynchronous with possibly some good thread count (probably one or two) and connect them to event pool or use callbacks
- [ ] Premium World Lock System (Used to purchase ingame goods)
- [ ] Broadcast, Banning, etc logs (sent to discord using webhooks)
- [ ] World Lock Algorithm (Sl, Bl, Hl, Wl, Dl, etc)

If you want to support the development of this project, make sure you contribute to this repo!

Make Sure To Check Out My Youtube Channel: (JenZip GT)
- https://www.youtube.com/channel/UCBpSmvDyThHz7obkqivKTLA