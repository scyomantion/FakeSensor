# FakeSensor

This Project is a DLL that can replace the
original dccmtr.dll from a Software that uses
the Spyder 4 Sensor. After replacement the
software can use the ArgyllCMS so you can use
many other Sensors.

## Dependencies

You need the ArgyllCMS on your Computer

## Disclaimer

I'm not responsible for anything that happens by using this project.
Everything happens at your own risk.

## Howto

Create a backup of the old dccmtr.dll of your software and after that replace it with the dll from this project.
If the software can't load the dll its maybe a x64 / x86 missmatch and you have to use the other one.
Copy the autocal.ini file in the same folder and change the path of the spotread.exe command to the location of your spotread.exe inside ArgyllCMS. 
You can get the latest ArgyllCMS from [https://www.argyllcms.com/](https://www.argyllcms.com/).
If you want to test it you can copy & paste the path into a cmd.exe window and check if everything works.

Please note that this project is in alpha status.

## Download

Download a precompiled Version from [Releases](https://github.com/scyomantion/FakeSensor/releases)

## Impressum

[Impressum](https://banjobyte.com/impressum.html)
