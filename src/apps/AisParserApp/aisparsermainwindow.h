#ifndef AISPARSERMAINWINDOW_H
#define AISPARSERMAINWINDOW_H

#include <QMainWindow>

//Input sources
//#include <AisInputSource.h>

//Output sources
//#include <AisWriter.h>

#include <AisDebug.h>

using namespace std;


namespace Ui {
class AisParserMainWindow;
}

class AisParserMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit AisParserMainWindow(QWidget *parent = 0);
    ~AisParserMainWindow();
protected slots:
    void manageInputVisibility(QString inputChoice);
    void manageOutputVisibility(QString outputChoice);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
    void directoryChooser();
    void fileChooser();
    void startParsingAis();
    void stopParsingAis();
    void displayLicense();
private:
    //int foo(AisInputSource* input, AisWriter* output);
    void updateProgress(unsigned int progress, QString message);
    template<class InputType, class OutputType, class AisSentenceParserType>
    int pointParser();

    template<class InputType, class OutputType, class AisSentenceParserType>
    int databaseParser();

    template<class InputType, class OutputType, class AisSentenceParserType>
    int trackParser();

    bool validateArguments();
    bool validateDatabaseArguments();
    bool validateFlatfileOutputArguments();
    bool validateFlatfileInputArguments();
    bool validateTcpInputArguments();

    void enableDatabaseFrame(bool enable);
    void enableTcpPortEntry(bool enable);
    bool m_continueParsing;

    Ui::AisParserMainWindow *ui;
};

#endif // AISPARSERMAINWINDOW_H
