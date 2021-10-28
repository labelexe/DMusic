import std/exitprocs, asyncdispatch
import cppbridge, qt, yandexMusicQmlModule, messages

when defined(unix):
  const pythonVersion = "3.9"
  {.passc: "-I/usr/include/python" & pythonVersion.}
  {.passl: "-L/usr/local/lib/python" & pythonVersion & " -lpython" & pythonVersion.}

  {.passc: "-I/usr/include/taglib".}
  {.passl: "-ltag".}

sourcesFromDir "src"
resourcesFromDir "."

{.emit: "#undef slots".}
{.emit: "#include <Python.h>".}
{.emit: "#define slots Q_SLOTS".}

{.emit: """#include "Translator.hpp"""".}

proc initializeDMusicQmlModule() {.importcpp: "initializeDMusicQmlModule()", header: "main.hpp".}
proc cppmain() {.importcpp: "cppmain()", header: "main.hpp".}

proc dmusic: string =
  {.emit: "Py_Initialize();".}
  {.emit: "Translator::setApp(QApplication::instance());".}

  QApplication.appName = "DMusic"
  QApplication.organizationName = "DTeam"
  QApplication.organizationDomain = "zxx.ru"

  initializeDMusicQmlModule()
  cppmain()

  let engine = newQQmlApplicationEngine()
  {.emit: "Translator::setEngine(&`engine`);".}
  engine.load "qrc:/qml/main.qml"

  onMainLoop:
    try: asyncdispatch.poll(5)
    except: discard

  setProgramResult QApplication.exec

  {.emit: "Py_Finalize();".}

when isMainModule:
  import cligen
  clcfg.version = "0.3"
  dispatch dmusic
