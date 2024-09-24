# ch36x api rw helper

A `Windows` program for helping `CH367`/`CH368` PCIe Driver access.


## Usaage

exec with this args:

```bash
  -i, --Index <int>          Device ID
  -a, --Action <string>      Action Option(mw/mr, er/ew)
  -f, --OutputFile <string>  output result to file
  -r, --Reg <string>         Reg for r/w
  -v, --Val <string>         Val for w
```


Memory R/W example:

```bash
xx.exe -i 0 -f result.txt -a mr -r 0x0000
xx.exe -i 0 -f result.txt -a mw -r 0x0000  -v 0x00000012
```

EEPROM R/W example:

```bash
: when reg 0x00 :   vid(16 Bit)
: when reg 0x01 :   did(16 Bit)
: when reg 0x02 :   rid( 8 Bit)
: when reg 0x04 :  svid(16 Bit)
: when reg 0x04 :   sid(16 Bit)

xx.exe -i 0 -f result.txt -a er -r 0x00
xx.exe -i 0 -f result.txt -a ew -r 0x00  -v 0x00000012
```


## For Developers

Build Env :

- `Visual C++ 6.0` with `ch36x_api_rw_helper.dsw`


