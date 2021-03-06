## Core Module

The Core module provides basic functionality which every Oryol app and Oryol module depends on:

* a unified application model across all target platforms
* lifetime management for heap-allocated objects
* a central logging facility
* a per-thread run-loop 
* selected alternatives for parts of the C++ std library which don't fit well into a realtime game application

### The Oryol Application Model

Oryol needs to run on all types of esoteric host platforms where applications may be forbidden to 
'own the game loop', block the main thread for more then a few dozen milliseconds, specific functionality
is only available from the main (or UI-) thread, don't have shared-memory-threading, don't
even have a main() function or where an app can be suspended and restored at will by 
the operating system.

To hide all these dirty platform-specific details, Oryol offers a convenient application
class *Oryol::App* which implements a simple *application-state-model*. During the lifetime
of an Oryol application, the app goes through different states, controlled by a derived 
application subclass. While a state is active, an associated per-frame method is called
which implements a frame's worth of work for the current application state. The only
hard rule is that these callback methods must not block for more then a few dozen milliseconds
(the usual frame budget is 16ms or 32ms).

Creating a new Oryol application starts with deriving an App subclass and overriding
the virtual state-callback methods which are called once per frame as long as the associated state
state is active. The return value of a state callback method is the next state the application
should switch to. 

A minimal Oryol application looks like this:

```cpp
#include "Core/App.h"

using namespace Oryol;

class MyApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();    
};
// this declares MyApp as the Oryol main class
OryolMain(MyApp);

//---: 1 frame initialization
AppState::Code MyApp::OnInit() {
    // do initialization work
    ...

    // returning AppState::Running switches the App to the Running state
    return AppState::Running;
}

//---: per-frame callback
AppState::Code MyApp::OnRunning() {
    // do per-frame work (e.g. 3D rendering)
    ...

    // either stay on Running state, or go into Cleanup state
    return done ? AppState::Cleanup : AppState::Running;
}

//---: 1 frame cleanup
AppState::Code MyApp::OnCleanup() {
    // do cleanup-work, calling parent class OnCleanup is currently necessary
    return App::OnCleanup();
}
```

> NOTE: the current model of implementing per-frame callbacks through virtual
> methods may change in the future


### Class Annotation Macros

When looking through Oryol header files you may wonder about macros inside
class declarations like this:

```cpp
class MyClass : public BaseClass {
    OryolClassDecl(MyClass);
    OryolClassCreator(MyClass);
public:
    /// default constructor
    MyClass();
    ...
};
```

*OryolClassDecl* and *OryolClassCreator*  are 'class annotation macros' which add some 
Oryol-specific standard functionality to a class. 


### Object Lifetime Management

> NOTE: in general, allocating objects on the heap should be avoided if possible,
> instead Oryol's philosophy is to pre-allocate fixed sized memory blocks and
> object pools and keep dynamic allocations to a minimum

Oryol applications should try to keep heap-allocated objects to a minimum, but if
needed, Oryol's lifetime-managed heap objects using ref-counting and smart pointers
should be used instead of raw new and delete calls.

There's a few things to keep in mind when using ref-counted objects in Oryol:

1. classes which use ref-counting should be subclasses of RefCounted (or provide a specific set of methods to manage the refcount, but that's an advanced topic)
2. the destructor must be virtual
3. a class annotation macro must be used to add creation methods to refcounted class
4. the Ptr<> class must be used as smart-pointer class

With this in place, creating an object then looks like this:

```cpp
Ptr<MyClass> myObj = MyClass::Create();
```

The *Create()* method is added by the OryolClassDecl() macro in the class declaration:

```cpp
class MyClass : public RefCounted {
    OryolClassDecl(MyClass);
public:
    ...
};
```

Non-default constructors can be invoked through the Create method as well, and
of course you can also use the new C++11 auto keyword:

```cpp
auto myObj = MyClass::Create(arg1, arg2, arg3);
```

> TODO: write about why std::shared_ptr and std::make_shared is not used.

> NOTE: Always keep in mind that there should be a good reason to use heap-allocated, 
> ref-counted objects instead of stack-allocated or class-embedded objects. Always consider
> stack-allocated objects and class-embedded objects first!


### Object Pools

Object creation can be optimized with object pools which prevents dynamic memory
allocation per object and reduces fragmentation (but still needs to call the constructor 
and destructor). This should only be used for small objects which need to be very frequently 
created and destroyed (which is something that should be avoided in a game engine). To use object 
pools for a class, use the OryolClassPoolAllocDecl annotation macro:

```cpp
class MyPoolClass : public MyClass {
    OryolClassPoolAllocDecl(MyClass);
public:
    ...  
};
```

The pool allocator will allocate objects in chunks of 256, up to 256 chunks, and will never
free claimed memory, so use this wisely.


### Deferred Object Creation

Sometimes the information of how to create an object must be handed around without actually
creating the object immediately. An object which knows how to create another object is
called a creator, and a class can be annotated to provide a static Creator() function
which returns a creator object:

```cpp
class MyClass : public RefCounted {
    OryolClassDecl(MyClass);
    OryolClassCreator(MyClass);
public:
    ... 
};
```

A creator is then created like this:

```cpp
// without constructor arguments:
auto creator = MyClass::Creator();

// with construction arguments
auto creator = MyClass::Creator(arg1, arg2, arg3);
```

A creator is actually a std::function object, so to create an object just invoke operator():

```cpp
// create object from previously created creator
Ptr<MyClass> myObj = creator();

// or more C++11 style:
auto myObj = creator(); 
```

### Logging

Oryol contains a central logging class **Log** with static logging methods. All Oryol text output goes through
this class. An application can attach custom logger objects derived from Logger to hook into the log messages.

Use the following methods for text output instead of printf:

```cpp
// normal text output
Log::Info("Hello World!");
// warning-level output
Log::Warn("A printf style log message %d...\n", blarghl);
// error-level output
Log::Error("...\n");
// debug-level output
Log::Dbg("Some spammy debug-only message\n");

// set the log level so that only warnings and errors will be displayed
Log::SetLogLevel(Log::Warn);
```

The Log class can be called safely from any thread.

### Asserts

Instead of assert(), use Oryol's specialized o_assert() macros, the standard form is 
also active when the code is compiled in release mode (with optimizations), the _dbg()
form is only active in debug mode:

```cpp
// standard assert, first form also active in release mode, second not:
o_assert(x != 0);
o_assert_dbg(x != 0);

// assert with additional text message:
o_assert2(x != 0, "Message");
o_assert2_dbg(x != 0, "Message");

// short-cut range check assert (tests that val >= 0 and val < max)
o_assert_range(val, max);
o_assert_range_dbg(val, max);
```

### The RunLoop

The main thread, and each thread created by Oryol has a 2 thread-local RunLoop objects,
one executed before the App's on-frame method, one after. An 
application (or Oryol modules) can attach std::function objects to the run loop so that
this function is automatically called once per frame.

Here's an example using C++11 lambdas:

```cpp
Core::PreRunLoop()->Add([] {
    Core::Log("Hello!\n");
});
```

In a proper Oryol App, this should now print 'Hello!' to stdout 60 times per second.

> NOTE: there's currently no control over the order of how RunLoop callbacks are executed in relation to each other.


### Things you should NOT use

There are a couple of C++ features which are black-listed on Oryol for various reasons:

- *C++ exceptions*: These are disabled by default, but can be enabled with a cmake option. Don't use them as they are fairly expensive on emscripten.
- *std containers (std::vector, std::map, ...)*: Oryol comes with its own set of containers classes which offer more fine-control over their behaviour and use asserts instead of exceptions.
- *C++ style input/output (e.g. std::cout << "hello")*: This causes a lot of code to be pulled into the executable, which is especially bad in emscripten where client-size matters.

