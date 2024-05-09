# Libhydrogen on CH32V003

- ## *What is this?*

### This is a proof of concept (PoC) project for using libhydrogen *properly* on WCH's CH32V003 microcontroller. 

The key to getting libhydrogen running correctly on **anything** is making sure you have a reasonable source of entropy to feed libhydrogen's true random number generator. This is done in this project by reading the internal bandgap voltage reference (Vrefint) with the ADC, and taking only the least significant bits (LSbs). There are other ways to collect entropy, but those will have to be for other projects.

- ## *How do I use it?*

### You integrate it into WCH's official development tool-chain. 

You would start by installing their free IDE, MounRiver Studio Community. Once installed, you would likely want to run through the wizard and set up a template project for the CH32V003, just to get all the project directories populated. After that, you would probably want to restart, plug in WCH's Link-e USB programmer/debugger, and attach that to a prototype board populated with a CH32V003. Alternatively, you could use a turn-key development board, where all the hardware is integrated. After you sanity checked that everything worked, and you can get successful programing/debugging, you would navigate to the project directory and copy/paste this repo into there, reload the IDE, and it should "just work™"

- ## "What should I expect to get when I'm done?"

### You should expect to get output like the following on whatever serial port your debugger/programmer puts data too.

![libhydrogen CH32V003-2](https://github.com/CharlesScoville/CH32V003F4P6-libhydrogen-PoC/assets/25935564/b0f7b255-5877-4219-bc1f-b72b5aa7712b)

- ## "What's the license? I thought libhydrogen was under the ISC License?"

### I reserve the right to every line of code I have written for this repo. Being independent, libhydrogen falls under their own license. Same goes for WCH's code.

That said, my retention of copyright for my work is strictly a countermeasure against bad actors. If you believe yourself to be in good faith, then you should not need to worry about this, as I will reciprocate good faith with good faith. If you have bad intentions, you should expect bad results. If you are concerned about this, contacting me to inform me of your intentions ahead of time is the safest bet. (This project is mostly intended to be used by libhydrogen devs for tests and development, so take that into consideration.)
