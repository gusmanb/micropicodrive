# MicroPicoDrive - An internal replacement of the Sinclair QL Microdrive.

## UPDATE 22/03/2023

The device has been tested and patched to work with Minerva. Yup!
Also, the PCB has been modified, it should now be correctly placed but I'm waiting to receive a new batch of PCB's, until then I can't ensure 100% that it is totally fixed.

The Wiki has been set in place with documentation on how to assemble and use the device.

The BOM has been added to the Hardware folder and contains links to the more uncommon components.

Have fun!

# Initial commit

WARNING! This is a work in progress, I need to test it with Minerva ROM and do some adjustments to the PCB, be ware!

So, I got a Sinclair QL some months ago, I wanted one for many many years, I'm a Sinclair fan and I always have been in the Spectrum world and the QL always has been something that I wanted to get.
The unit I got was a broken one, it was stored since the 80's in a repair shop, when I got it the machine was non-working, without microdrives, with a faulty membrane and without a single screw, so the first that I did was to repair it.

And once I got it fully working I wanted to load something, but without microdrives it was a bit difficult... xD

I checked which solutions were available but I didn't liked none so I decided to create my own replacement, armed with my [logic analizer](https://github.com/gusmanb/logicanalyzer) and patience I learnt the internals of the microdrive protocol and created an internal replacement that uses "cartridges" and can be installed in the internal space of the QL.

And this is what I got:

![IMG_20230919_020735](https://github.com/gusmanb/micropicodrive/assets/4086913/33d1c477-5055-4630-a182-1f720620ed9e)
![IMG_20230919_020740](https://github.com/gusmanb/micropicodrive/assets/4086913/d03f2089-6421-410a-b68f-075cdda09699)

The device is based in an RP2040 (I love the PIO units, those damn things are a wonder!) and some level shifters... and that's it. Well, that's it for the internal unit, there is also a screen, some buttons and an SD card holder for the cartridge!

![IMG_20230919_021006](https://github.com/gusmanb/micropicodrive/assets/4086913/a702383c-78bb-46b4-889f-02ecd6c4707d)
![IMG_20230919_021144](https://github.com/gusmanb/micropicodrive/assets/4086913/b51687d6-f8bf-4490-ace4-03ce6e5b20b0)

The thing is completelly working with the stock rom, it can load programs without problems, can format cartridges, store images, etc, all without problems.

![IMG_20230919_021200](https://github.com/gusmanb/micropicodrive/assets/4086913/f3bd98f9-7f8e-4986-9b22-3d015cdf9be5)
![IMG_20230919_021312](https://github.com/gusmanb/micropicodrive/assets/4086913/9525a51d-6cf1-495c-8d3a-bfe2cf339633)

Also, I found that a lot of the software is stored as Zip files, the device right now supports MDV, IMG and my own custom format, so I checked the available utilities to manipulate MDV images and... it was a boomer to find that there is no updated one, only mdvtools that is ancient and after compiling manually it I found that most of the converted images were flawed.

So, again, I decided to create mi own solution, and this is what I finally got:

![imagen](https://github.com/gusmanb/micropicodrive/assets/4086913/4f412604-17ac-4195-a32c-49bdbf13fb4a)
![imagen](https://github.com/gusmanb/micropicodrive/assets/4086913/f6bb86fa-ec53-434f-914e-1e64be1dfd91)

It can create MDV images from the ground, import MDV and ZIP files (and yes, it supports the QL zip format, it preserves the executable and data space attributes), manipulate the existing files, extract them, etc, so with this finally I got a full toolset for the QL!

Now I'm waiting for the components to build a 512Kb RAM expansion and from there I don't know where I will continue, may be a floppy drive emulator? IDE interface? Not sure, but the first is to add those 512Kb and a Minerva ROM to test the microdrive.

In any case, this is a work in progress, beware that I need to do some adjustments to the PCB and finish some tools for the software.

If you want more info don't esitate to contact me.

Have fun!
