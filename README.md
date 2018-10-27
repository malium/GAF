# GAF
GAF (Greaper Application Framework) is a library which is an application framework that provides all the needed functionality to the applications by being a framework between OS and the application.

Right now this framework just supports Windows 7/8/8.1/10 in it's 64 bits, in the future will support MacOSX and Linux (at least Ubuntu and FreeBSD) and Windows with 32bit.

I've created this project in order to abstract all the OS stuff to help in my other projects. I wish it helps you too and with the help of the community improve this until it becomes a robust application framework library.

# Why to use GAF?
Well I'm sure that there are good application frameworks out there, but I've created this one in order to create a Youtube channel to teach programing and also to teach myself how everything works under the hood. GAF also is aimed to have the best performace possible (as far as I could) and give all the customization that someone may want.

# This Framework includes:
- A PHAL(Platfrom Hardware Abstraction Layer) which detects your compiler and it's version and your Operating System, it also create an abstraction of the basic types and provides some definitions and utilites that can benefit from the different OSs and compilers.
- An Application Manager that handles all the different managers, also it is a task manager and a module manager.
- An Assertion abstraction which makes assertions easy to use and to display, it also managers dump creation.
- A CommandSystem that allows command sending to the different available Commands through a Console which is also managed by this System.
- A CryptoAPI which allows to add your custom Crypto system and use the provided one which is very simple but very fast.
- An EventManager which allows to send events and listen to them easily.
- A FileSystem which simplifies the file and Directory management.
- A TestFramework which helps to test the different Greaper libraries.
- A HWDetector, which detects the different features of your CPU, your OS name and Version, and the available RAM.
- An ImageManager (Under development), which use FreeImage to load and store images.
- An InputManager, that let's you receive input events easily.
- A LogManager, which let's you log anything an handle log messages as you want, it also provides a DefaultLog.
- A MainHelper (Main abstraction) which abstracts the main to be cross-platform and also does some things which has to be done before anything is done.
- An MPMCQueue, which provides an easy way to handle things in a MultipleProducer-MultipleConsumer way.
- A PropertiesManager, which as it's name says, manages properties, which are similar to CVars.
- A BinarySerializer, to enable binary serializing of classes and also gives you a way to create a custom binary serial.
- A ResourceManager (Under development), which allows to load resources and manage it's life time.
- A TaskManager framework, which provides methods to send tasks to it and also to attach a number of threads to a certain System/Manager.
- A Version framework, which gives the current version of GAF or any version using this framework.
- A Window Manager, which helps you to create windows and also to manage them easily.
- Also comes with a set of utilites.

# Why is under Apache 2.0 License?
Well I wanted to release it as an OpenSource project but also that when someone add some functionality or modify one, I will see what they have done, so I may ask them to include it to the official branch and keep this project forward.

# What needs to be finished?
- ResourceManager needs to be tested.
- ImageManager needs to be finished.
- Add a test file which shows some kind of normal use of this application.
- Finish to comment all.
- Add more debbuging tools.
- Make everything truly cross-platform.
- Add other resources importer/exporters in order to make it more versatile.

# How to contribute?
Well I will apreciate a lot any contribution to make this project better. I will provide a mail to ask me anything about it or if you want to comment something.

# How to use GAF? 
Well right now there are no tutorials on how to use it but soon will be. Stay tuned for the updates.
