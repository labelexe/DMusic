#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QQuickStyle>
#include <QQuickView>
#include <QWidget>
#include "yapi.hpp"
#include "file.hpp"
#include "Config.hpp"
#include "AudioPlayer.hpp"
#include "RemoteMediaController.hpp"
#include "Clipboard.hpp"
#include "DFileDialog.hpp"
#include "Messages.hpp"
#include "ConsoleArgs.hpp"
#include "Translator.hpp"

// TODO: dislike
// TODO: Память течёт, хоть и понемногу (потоковый режим)
// TODO: Добавить возможность отменять загрузку обложки и аудио для трека (например, при переключении на следующий)

int main(int argc, char *argv[])
{
  ConsoleArgs args(argc, argv);

  Py_Initialize();

	QApplication app(argc, argv);

	Translator::setApp(&app);
	Translator::instance->setLanguage(Config::language());
	QObject::connect(Config::instance, &Config::languageChanged, [](Config::Language language) {
		Translator::instance->setLanguage(language);
	});

//  bool gui = args.count() == 0 || args.has("-g") || args.has("--gui");

  if (args.has("-v") || args.has("--version")) {
    std::cout << "DMusic 0.1" << std::endl;
  }

  if (args.has("-h") || args.has("--help")) {
    std::cout << QObject::tr(
"DMusic - music player\n"
"usage: %1 [options]\n\n"
"-h --help     show help\n"
"-v --version  show version\n"
"-g --gui      run application\n"
"-verbose      show more logs\n"
    ).arg(argv[0]) << std::endl;
  }

  if (!args.has("--verbose")) {
    try {
      auto logging = py::module("logging");
      auto logger = logging.call("getLogger");
      logger.call("setLevel", logging.get("CRITICAL"));
    } catch (py::error& e) {
      Messages::error("a", e.what());
    }
  }

//  if (!gui) return 0;

	QQuickStyle::setStyle("Material");

  qmlRegisterType<YTrack>("DMusic", 1, 0, "YTrack");
  qmlRegisterType<YArtist>("DMusic", 1, 0, "YArtist");
  qmlRegisterType<YTrack>("DMusic", 1, 0, "Track");
  qmlRegisterType<Playlist>("DMusic", 1, 0, "Playlist");
  qmlRegisterType<DPlaylist>("DMusic", 1, 0, "DPlaylist");
  qmlRegisterType<Radio>("DMusic", 1, 0, "Radio");
  qmlRegisterType<PlaylistRadio>("DMusic", 1, 0, "PlaylistRadio");
  qmlRegisterType<AudioPlayer>("DMusic", 1, 0, "AudioPlayer");
  qmlRegisterType<RemoteMediaController>("DMusic", 1, 0, "RemoteMediaController");
  qmlRegisterType<Clipboard>("DMusic", 1, 0, "Clipboard");
  qmlRegisterType<DFileDialog>("DMusic", 1, 0, "DFileDialog");
  qmlRegisterType<YPlaylist>("DMusic", 1, 0, "YPlaylist");
  qmlRegisterType<YLikedTracks>("DMusic", 1, 0, "YLikedTracks");
  qmlRegisterType<YPlaylistsModel>("DMusic", 1, 0, "YPlaylistsModel");

	qmlRegisterSingletonType<Config>("DMusic", 1, 0, "Config", &Config::qmlInstance);
	qmlRegisterSingletonType<Config>("Config", 1, 0, "Config", &Config::qmlInstance);
	qmlRegisterSingletonType<Translator>("DMusic", 1, 0, "Translator", &Config::qmlInstance);
  qmlRegisterSingletonType<Messages>("DMusic", 1, 0, "Messages", &Messages::qmlInstance);
  qmlRegisterSingletonType<YClient>("DMusic", 1, 0, "YClient", &YClient::qmlInstance);

	qmlRegisterSingletonType(QUrl("qrc:/qml/StyleSingleton.qml"), "DMusic", 1, 0, "Style");

#ifdef Q_OS_LINUX
  QGuiApplication::setWindowIcon(QIcon(":resources/app-papirus.svg"));
#else
  QGuiApplication::setWindowIcon(QIcon(":resources/app.svg"));
#endif
  app.setApplicationName("DMusic");
  app.setOrganizationName("DTeam");
  app.setOrganizationDomain("zxx.ru");

	QQmlApplicationEngine engine;
	Translator::setEngine(&engine);
	engine.load(QUrl("qrc:/qml/main.qml"));

  auto r = app.exec();
  Py_Finalize();
  return r;
}
