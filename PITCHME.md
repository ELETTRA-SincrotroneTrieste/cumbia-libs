### Elettra - Sincrotrone Trieste

# Cumbia

#### A new multi threaded framework to develop [Tango] applications

#### Giacomo Strangolino

giacomo.strangolino@elettra.eu

---
@title[Why replace QTango]

@ul
- It has grown older...
- Lots of features not required daily
- Some useful features not easy to implement (e.g. multiple serialized readings)
- Tightly bound to Tango
- Has been stable for years, but the architecture is somehow complicated
- Code is not modular nor reusable enough
@ulend

![](assets/img/presentation.png)

---
@title[Cumbia]

@snap[north span-90]
@box[bg-orange text-white rounded demo-box-pad](Cumbia is a dance-oriented music genre popular throughout Latin America. It began as a courtship dance practiced among the African population on the Caribbean coasts of Colombia. It later mixed with Amerindian and European instruments, steps and musical characteristics. Cumbia has grown to be one of the most widespread and unifying musical genres to emerge from Latin America.)
@snapend

@snap[south]
 @img[shadow](assets/img/modules.png)
@snapend


---
@title[Keywords and technologies]

@snap [east span-40]
@ul
- C++ 11
- Qt
- Qt widgets
- Qt QML
- Qt Quick
- Tango
- Epics
- Websocket
- Android
- Linux
@ulend
@snapend

@snap [west span-60]
#### cumbia is written in C++ and uses the Qt libraries to develop graphical user interfaces. This means cumbia applications work on desktop computers as well as on Android devices
@snapend

@snap [south span-100]
@img[shadow](assets/img/android-plot1.png)
@snapend

