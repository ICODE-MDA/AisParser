#include "aisparsermainwindow.h"
#include "ui_aisparsermainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QSettings>

#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/timer/timer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

//Input Sources
#include <AisFlatFileInputSource.h>
#include <AisTcpStreamInputSource.h>

//Output Sources
#include <AisTsvWriter.h>
#include <AisCsvWriter.h>
#include <AisDatabaseWriter.h>
#include <AisMySqlDatabaseWriter.h>
#include <AisPostgreSqlDatabaseWriter.h>
#include <AisKmlPlacemarkWriter.h>
#include <AisKmlTrackWriter.h>
#include <AisShapefilePointWriter.h>
#include <AisShapefileTrackWriter.h>

//Parsers
#include <AisMsisSentenceParser.h>
#include <AisSatSentenceParser.h>
#include <AisMessageParser.h>

#include <AisMessage.h>

AisParserMainWindow::AisParserMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AisParserMainWindow),
    m_continueParsing(true)
{
    ui->setupUi(this);

    ui->statusbar->showMessage("Idle...");
    connect(ui->actionLicense, SIGNAL(triggered()), this, SLOT(displayLicense()));
    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));

	setAcceptDrops(true);		//enables app window to accept dropped in elements, to support file/directory drag and drop input

	readSettings();
}

AisParserMainWindow::~AisParserMainWindow()
{
	writeSettings();
    delete ui;
}


void AisParserMainWindow::writeSettings()

{
	QSettings settings;
	settings.beginGroup("AisParserMainWindow");
	settings.setValue("inputFile", ui->filenameLineEdit->text());
	settings.setValue("tcp_hostname", ui->inputHostnameLineEdit->text());
	settings.setValue("tcp_port", ui->portSpinBox->value());
	settings.setValue("messages_per_file", ui->messagesPerFileSpinBox->value());
	settings.setValue("outputDir", ui->outputDirectoryLineEdit->text());
	settings.setValue("database_username", ui->usernameLineEdit->text());
	//settings.setValue("database_password", ui->passwordLineEdit->text());
	settings.setValue("database_hostname", ui->hostnameLineEdit->text());
	settings.setValue("database_name", ui->databaseNameLineEdit->text());
	settings.setValue("database_table", ui->tableNameLineEdit->text());
	settings.setValue("database_numiterations", ui->numIterationsSpinBox->value());
	settings.endGroup();
}

 

void AisParserMainWindow::readSettings()

{
	//start settings
	QSettings settings;
	settings.beginGroup("AisParserMainWindow");
	ui->filenameLineEdit->setText(settings.value("inputFile", "").toString());
	ui->inputHostnameLineEdit->setText(settings.value("tcp_hostname", "localhost").toString());
	ui->portSpinBox->setValue(settings.value("tcp_port", 10000).toInt());
	ui->messagesPerFileSpinBox->setValue(settings.value("messages_per_file", 0).toInt());
	ui->outputDirectoryLineEdit->setText(settings.value("outputDir", "").toString());
	ui->usernameLineEdit->setText(settings.value("database_username", "").toString());
	ui->hostnameLineEdit->setText(settings.value("database_hostname", "").toString());
	ui->databaseNameLineEdit->setText(settings.value("database_name", "").toString());
	ui->tableNameLineEdit->setText(settings.value("database_table","").toString());
	ui->numIterationsSpinBox->setValue(settings.value("database_numiterations", 0).toInt());
	//end settings
	settings.endGroup();
}

 
void AisParserMainWindow::displayLicense()
{
    QMessageBox msgBox;
    msgBox.setText("License: The MIT License (MIT)\n"
                   "Copyright (c) 2012 ICODE-MDA\n"
                   "Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n"
                   "The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n"
                   "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
    msgBox.exec();
    return;
}

//Overloaded/reimplemented function to handle dragged in file/directory events
void AisParserMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasUrls())
		event->acceptProposedAction();
}

//Overloaded/reimplemented function to handle dropped in file/directory events
void AisParserMainWindow::dropEvent(QDropEvent *event)
{
	foreach(QUrl url, event->mimeData()->urls())
	{
		QString urltext = url.toLocalFile();
		QString suffix = QFileInfo(urltext).suffix().toUpper();
		//If dropped in element is a file (specifically, a "log" file), then use URL as input
		if(suffix == "LOG")
		{
			event->acceptProposedAction();
			ui->filenameLineEdit->setText(urltext);
			continue;
		}
		//If dropped in element is a directory, use the URL as output directory
		else if (suffix == "")
		{
			event->acceptProposedAction();
			ui->outputDirectoryLineEdit->setText(urltext);
			continue;
		}
	}

	event->acceptProposedAction();
}

void AisParserMainWindow::fileChooser()
{
	QString filename = QFileDialog::getOpenFileName();
	ui->filenameLineEdit->setText(filename);
}

void AisParserMainWindow::directoryChooser()
{
    QString directory = QFileDialog::getExistingDirectory();
    ui->outputDirectoryLineEdit->setText(directory);
}

void AisParserMainWindow::enableDatabaseFrame(bool enable)
{
    ui->databaseInformationFrame->setEnabled(enable);
    ui->databaseInformationLabel->setEnabled(enable);
    ui->messagesPerFilelabel->setEnabled(!enable);
    ui->messagesPerFileSpinBox->setEnabled(!enable);
    ui->outputDirectoryLabel->setEnabled(!enable);
    ui->outputDirectoryToolButton->setEnabled(!enable);
    ui->outputDirectoryLineEdit->setEnabled(!enable);
}

void AisParserMainWindow::enableTcpPortEntry(bool enable)
{
    ui->portLabel->setEnabled(enable);
    ui->portSpinBox->setEnabled(enable);
    ui->inputHostnameLabel->setEnabled(enable);
    ui->inputHostnameLineEdit->setEnabled(enable);

    ui->filenameLabel->setEnabled(!enable);
    ui->filenameToolButton->setEnabled(!enable);
    ui->filenameLineEdit->setEnabled(!enable);
}

void AisParserMainWindow::manageInputVisibility(QString inputChoice)
{
    if(inputChoice == "Terrestrial AIS TCP" || inputChoice == "Satellite AIS TCP")
    {
        enableTcpPortEntry(true);
    }
    else
    {
        enableTcpPortEntry(false);
    }
}

void AisParserMainWindow::manageOutputVisibility(QString outputChoice)
{
    if(outputChoice == "MySql Database" || outputChoice == "Oracle Database" || outputChoice == "PostgreSQL Database")
    {
        enableDatabaseFrame(true);
    }
    else
    {
        enableDatabaseFrame(false);
    }
}

bool AisParserMainWindow::validateDatabaseArguments()
{
    return true;
}

bool AisParserMainWindow::validateFlatfileOutputArguments()
{
    if(ui->outputDirectoryLineEdit->displayText() == "")
    {
        QMessageBox msgBox;
        msgBox.setText("Please specify an output directory");
        msgBox.exec();
        return false;
    }

    return true;
}

bool AisParserMainWindow::validateFlatfileInputArguments()
{
    if(ui->filenameLineEdit->displayText() == "")
    {
        QMessageBox msgBox;
        msgBox.setText("Please specify an input file");
        msgBox.exec();
        return false;
    }

    return true;
}
bool AisParserMainWindow::validateTcpInputArguments()
{
    return true;
}

bool AisParserMainWindow::validateArguments()
{
    QString outputType = ui->outputTypeComboBox->currentText();
    QString inputType = ui->inputTypeComboBox->currentText();
    bool outputArgumentsValid = false;
    bool inputArgumentsValid = false;
    if(outputType == "MySql Database" || outputType == "Oracle Database" || outputType == "PostgreSQL Database")
    {
        outputArgumentsValid = validateDatabaseArguments();
    }
    else
    {
        outputArgumentsValid = validateFlatfileOutputArguments();
    }

    if(inputType == "Satellite AIS Log" || inputType == "Terrestrial AIS Log")
    {
        inputArgumentsValid = validateFlatfileInputArguments();
    }
    else
    {
        inputArgumentsValid = validateTcpInputArguments();
    }

    return outputArgumentsValid && inputArgumentsValid;
}

void AisParserMainWindow::startParsingAis()
{
    m_continueParsing = true;
    bool validArguments = validateArguments();
    if(!validArguments)
    {
        //        QMessageBox msgBox;
        //        msgBox.setText("The inputs specified are not valid, please correct this and try again");
        //        msgBox.exec();
        return;
    }

    QString inputChoice = ui->inputTypeComboBox->currentText();
    QString outputChoice = ui->outputTypeComboBox->currentText();

    if( inputChoice == "Satellite AIS Log")
    {
        if(outputChoice == "Tab-Seperated Files")
        {
            pointParser<AisFlatFileInputSource, AisTsvWriter, AisSatSentenceParser>();
        }
		else if(outputChoice == "Comma-Seperated Files")
        {
            pointParser<AisFlatFileInputSource, AisCsvWriter, AisSatSentenceParser>();
        }
		else if(outputChoice == "PostgreSQL Database")
        {
            databaseParser<AisFlatFileInputSource, AisPostgreSqlDatabaseWriter, AisSatSentenceParser>();
        }
        else if(outputChoice == "MySql Database")
        {
            databaseParser<AisFlatFileInputSource, AisMySqlDatabaseWriter, AisSatSentenceParser>();
        }
        else if(outputChoice == "Oracle Database")
        {
            databaseParser<AisFlatFileInputSource, AisDatabaseWriter, AisSatSentenceParser>();
        }
        else if(outputChoice == "KML Placemarks")
        {
            pointParser<AisFlatFileInputSource, AisKmlPlacemarkWriter, AisSatSentenceParser>();
        }
        else if(outputChoice == "KML Tracks")
        {
            trackParser<AisFlatFileInputSource, AisKmlTrackWriter, AisSatSentenceParser>();
        }
        else if(outputChoice == "ESRI Shapefile Points")
        {
            pointParser<AisFlatFileInputSource, AisShapefilePointWriter, AisSatSentenceParser>();
        }
        else if(outputChoice == "ESRI Shapefile Tracks")
        {
            trackParser<AisFlatFileInputSource, AisShapefileTrackWriter, AisSatSentenceParser>();
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Error, unspecified output type. Please correct this and try again");
            msgBox.exec();
            return;
        }
    }
    else if( inputChoice == "Terrestrial AIS Log")
    {
        if(outputChoice == "Tab-Seperated Files")
        {
            pointParser<AisFlatFileInputSource, AisTsvWriter, AisMsisSentenceParser>();
        }
		else if(outputChoice == "Comma-Seperated Files")
        {
            pointParser<AisFlatFileInputSource, AisCsvWriter, AisMsisSentenceParser>();
        }
		else if(outputChoice == "PostgreSQL Database")
        {
            databaseParser<AisFlatFileInputSource, AisPostgreSqlDatabaseWriter, AisMsisSentenceParser>();
        }
        else if(outputChoice == "MySql Database")
        {
            databaseParser<AisFlatFileInputSource, AisMySqlDatabaseWriter, AisMsisSentenceParser>();
        }
        else if(outputChoice == "Oracle Database")
        {
            databaseParser<AisFlatFileInputSource, AisDatabaseWriter, AisMsisSentenceParser>();
        }
        else if(outputChoice == "KML Placemarks")
        {
            pointParser<AisFlatFileInputSource, AisKmlPlacemarkWriter, AisMsisSentenceParser>();
        }
        else if(outputChoice == "KML Tracks")
        {
            trackParser<AisFlatFileInputSource, AisKmlTrackWriter, AisMsisSentenceParser>();
        }
        else if(outputChoice == "ESRI Shapefile Points")
        {
            pointParser<AisFlatFileInputSource, AisShapefilePointWriter, AisMsisSentenceParser>();
        }
        else if(outputChoice == "ESRI Shapefile Tracks")
        {
            trackParser<AisFlatFileInputSource, AisShapefileTrackWriter, AisMsisSentenceParser>();
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Error, unspecified output type. Please correct this and try again");
            msgBox.exec();
            return;
        }
    }
    else if (inputChoice == "Satellite AIS TCP")
    {
        if(outputChoice == "Tab-Seperated Files")
        {
            pointParser<AisTcpStreamInputSource, AisTsvWriter, AisSatSentenceParser>();
        }
		else if(outputChoice == "Comma-Seperated Files")
        {
            pointParser<AisTcpStreamInputSource, AisCsvWriter, AisSatSentenceParser>();
        }
		else if(outputChoice == "PostgreSQL Database")
        {
            databaseParser<AisTcpStreamInputSource, AisPostgreSqlDatabaseWriter, AisSatSentenceParser>();
        }
        else if(outputChoice == "MySql Database")
        {
            databaseParser<AisTcpStreamInputSource, AisMySqlDatabaseWriter, AisSatSentenceParser>();
        }
        else if(outputChoice == "Oracle Database")
        {
            databaseParser<AisTcpStreamInputSource, AisDatabaseWriter, AisSatSentenceParser>();
        }
        else if(outputChoice == "KML Placemarks")
        {
            pointParser<AisTcpStreamInputSource, AisKmlPlacemarkWriter, AisSatSentenceParser>();
        }
        else if(outputChoice == "KML Tracks")
        {
            trackParser<AisTcpStreamInputSource, AisKmlTrackWriter, AisSatSentenceParser>();
        }
        else if(outputChoice == "ESRI Shapefile Points")
        {
            pointParser<AisTcpStreamInputSource, AisShapefilePointWriter, AisSatSentenceParser>();
        }
        else if(outputChoice == "ESRI Shapefile Tracks")
        {
            trackParser<AisTcpStreamInputSource, AisShapefileTrackWriter, AisSatSentenceParser>();
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Error, unspecified output type. Please correct this and try again");
            msgBox.exec();
            return;
        }
    }
    else if (inputChoice == "Terrestrial AIS TCP")
    {
        if(outputChoice == "Tab-Seperated Files")
        {
            pointParser<AisTcpStreamInputSource, AisTsvWriter, AisMsisSentenceParser>();
        }
		else if(outputChoice == "Comma-Seperated Files")
        {
            pointParser<AisTcpStreamInputSource, AisCsvWriter, AisMsisSentenceParser>();
        }
		else if(outputChoice == "PostgreSQL Database")
        {
            databaseParser<AisTcpStreamInputSource, AisPostgreSqlDatabaseWriter, AisMsisSentenceParser>();
        }
        else if(outputChoice == "MySql Database")
        {
            databaseParser<AisTcpStreamInputSource, AisMySqlDatabaseWriter, AisMsisSentenceParser>();
        }
        else if(outputChoice == "Oracle Database")
        {
            databaseParser<AisTcpStreamInputSource, AisDatabaseWriter, AisMsisSentenceParser>();
        }
        else if(outputChoice == "KML Placemarks")
        {
            pointParser<AisTcpStreamInputSource, AisKmlPlacemarkWriter, AisMsisSentenceParser>();
        }
        else if(outputChoice == "KML Tracks")
        {
            trackParser<AisTcpStreamInputSource, AisKmlTrackWriter, AisMsisSentenceParser>();
        }
        else if(outputChoice == "ESRI Shapefile Points")
        {
            pointParser<AisTcpStreamInputSource, AisShapefilePointWriter, AisMsisSentenceParser>();
        }
        else if(outputChoice == "ESRI Shapefile Tracks")
        {
            trackParser<AisTcpStreamInputSource, AisShapefileTrackWriter, AisMsisSentenceParser>();
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Error, unspecified output type. Please correct this and try again");
            msgBox.exec();
            return;
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Error, unspecified input type. Please correct this and try again");
        msgBox.exec();
        return;
    }



}

void AisParserMainWindow::stopParsingAis()
{
    m_continueParsing = false;
}



//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
void AisParserMainWindow::updateProgress(unsigned int progress, QString message)
{
    if(progress%10000 == 0)
    {
        ui->statusbar->showMessage(message);
    }
    else if(progress%1000 == 0)
    {
        QString currentMessage = ui->statusbar->currentMessage();
        ui->statusbar->showMessage(currentMessage + ".");
    }
}

template<class InputType, class OutputType, class AisSentenceParserType>
int AisParserMainWindow::pointParser()
{
    QString statusMessage("Parsing AIS point-by-point...");
    ui->statusbar->showMessage(statusMessage);
    string inputType = ui->inputTypeComboBox->currentText().toStdString();
    std::string filename;
    if(inputType == "Satellite AIS TCP" || inputType == "Terrestrial AIS TCP")
    {
        filename = ui->hostnameLineEdit->text().toStdString() + ":" + boost::lexical_cast<std::string>(ui->portSpinBox->value());
    }
    else
    {
        filename = ui->filenameLineEdit->displayText().toStdString();
    }
    boost::timer::auto_cpu_timer timer;

    unsigned int messagesPerFile = ui->messagesPerFileSpinBox->value();

    //Define input class (an AisInputSource)
    //STEPX: choose the correct type of input source
    InputType aisInputSource(filename);

    unsigned int partition = 0;
    unsigned int progress = 1;

    while(aisInputSource.isReady())
    {

        //Define output class (an AisWriter)
        //STEPX: choose the correct type of output source
        QFileInfo qfileinfo(filename.c_str());
        std::string fileSuffix = qfileinfo.completeSuffix().toStdString();
        std::string inputFilename = ui->outputDirectoryLineEdit->text().toStdString() + "/" + fileSuffix + ".p" + boost::lexical_cast<string>(partition++);
        OutputType aisWriter(inputFilename);

        if(!aisWriter.isReady())
        {
            aisDebug("AisWriter is not ready");
            return -1;
        }

        for(unsigned int messageCount = 0; ((messagesPerFile == 0) || (messageCount < messagesPerFile)) && (aisInputSource.isReady()) && m_continueParsing; messageCount++)
        {
            updateProgress(progress++, statusMessage);

            //load the next sentence from the AIS input to the parser
            //STEPX: choose the correct type of sentence parser
            AisSentenceParserType aisSentenceParser(aisInputSource.getNextSentence());
            AisMessageParser aisMessageParser;

            if(aisSentenceParser.isMessageValid())
            {
                //This check is to make sure that if the first sentence of the message
                //was bad we won't read the second sentence and parse it as a new message
                if(aisSentenceParser.getSentenceNumber()==1)
                {
                    aisMessageParser.addData(aisSentenceParser.getData());
                    //if the current sentence is part of a multipart message
                    //grab the next message until you have them all, or message is invalid
                    try
                    {
                        while(aisSentenceParser.getSentenceNumber() < aisSentenceParser.getNumberOfSentences())
                        {
                            aisSentenceParser.setSentence(aisInputSource.getNextSentence());
                            if(aisSentenceParser.isMessageValid()){
                                aisMessageParser.addData(aisSentenceParser.getData());
                            }
                            else
                            {
                                //aisDebug("Invalid multipart message:\n" + aisSentenceParser.getCurrentSentence());
                                throw std::runtime_error("Invalid multipart message");
                            }
                        }

                        AisMessage aisMessage = aisMessageParser.parseMessage();
                        //add time from ais sentence to the ais message
                        aisMessage.setDATETIME(aisSentenceParser.getTimestamp());
                        //add streamid from ais sentence to the ais message
                        aisMessage.setSTREAMID(aisSentenceParser.getStreamId());

                        aisWriter.writeEntry(aisMessage);
                    }
                    catch(exception &e)
                    {
                        cerr << e.what() << endl;
                    }
                }
                else
                {
                    aisDebug("First sentence of message was invalid/not receieved.\nSkipping the rest of the sentences of this message");
                    continue;
                }
            }
            else
            {
                //aisDebug("Invalid message:\n" + aisSentenceParser.getCurrentSentence());
            }
        }
    }
    ui->statusbar->showMessage("Done parsing AIS...");
    return 0;
}


template<class InputType, class OutputType, class AisSentenceParserType>
int AisParserMainWindow::trackParser()
{
    QString statusMessage("Parsing AIS into tracks...");
    ui->statusbar->showMessage(statusMessage);
    boost::timer::auto_cpu_timer timer;
    string filename;
    string inputType = ui->inputTypeComboBox->currentText().toStdString();
    if(inputType == "Satellite AIS TCP" || inputType == "Terrestrial AIS TCP")
    {
        filename = ui->hostnameLineEdit->text().toStdString() + ":" + boost::lexical_cast<std::string>(ui->portSpinBox->value());
    }
    else
    {
        filename = ui->filenameLineEdit->displayText().toStdString();
    }

    unsigned int tracksPerFile = ui->messagesPerFileSpinBox->value();
    unsigned int progress = 1;

    //Define input class (an AisInputSource)
    //STEPX: choose the correct type of input source
    InputType aisInputSource(filename);

    QFileInfo qfileinfo(filename.c_str());
    std::string fileSuffix = qfileinfo.completeSuffix().toStdString();
    std::string inputFilename = ui->outputDirectoryLineEdit->text().toStdString() + "/" + fileSuffix;
    OutputType aisWriter(inputFilename, tracksPerFile);

    while(aisInputSource.isReady())
    {
        while(aisInputSource.isReady() && m_continueParsing)
        {
            updateProgress(progress++, statusMessage);
            //load the next sentence from the AIS input to the parser
            //STEPX: choose the correct type of sentence parser
            AisSentenceParserType aisSentenceParser(aisInputSource.getNextSentence());
            AisMessageParser aisMessageParser;

            if(aisSentenceParser.isMessageValid())
            {
                //This check is to make sure that if the first sentence of the message
                //was bad we won't read the second sentence and parse it as a new message
                if(aisSentenceParser.getSentenceNumber()==1)
                {
                    aisMessageParser.addData(aisSentenceParser.getData());
                    //if the current sentence is part of a multipart message
                    //grab the next message until you have them all, or message is invalid
                    try
                    {
                        while(aisSentenceParser.getSentenceNumber() < aisSentenceParser.getNumberOfSentences())
                        {
                            aisSentenceParser.setSentence(aisInputSource.getNextSentence());
                            if(aisSentenceParser.isMessageValid()){
                                aisMessageParser.addData(aisSentenceParser.getData());
                            }
                            else
                            {
                                //aisDebug("Invalid multipart message:\n" + aisSentenceParser.getCurrentSentence());
                                throw std::runtime_error("Invalid multipart message");
                            }
                        }

                        AisMessage aisMessage = aisMessageParser.parseMessage();
                        //add time from ais sentence to the ais message
                        aisMessage.setDATETIME(aisSentenceParser.getTimestamp());
                        //add streamid from ais sentence to the ais message
                        aisMessage.setSTREAMID(aisSentenceParser.getStreamId());
                        //aisWriter.writeEntry(aisMessage);
                        aisWriter.writeEntry(aisMessage);
                    }
                    catch(exception &e)
                    {
                        cerr << e.what() << endl;
                    }
                }
                else
                {
                    aisDebug("First sentence of message was invalid/not receieved.\nSkipping the rest of the sentences of this message");
                    continue;
                }
            }
            else
            {
                //aisDebug("Invalid message:\n" + aisSentenceParser.getCurrentSentence());
            }
        }
    }

    ui->statusbar->showMessage("Writing AIS tracks to file...");
    aisWriter.writeToFile();
    ui->statusbar->showMessage("Done parsing AIS...");
    return 0;
}

template<class InputType, class OutputType, class AisSentenceParserType>
int AisParserMainWindow::databaseParser()
{
    QString statusMessage("Parsing AIS into a database...");
    ui->statusbar->showMessage(statusMessage);
    boost::timer::auto_cpu_timer timer;
    string filename;
    string inputType = ui->inputTypeComboBox->currentText().toStdString();
    if(inputType == "Satellite AIS TCP" || inputType == "Terrestrial AIS TCP")
    {
        filename = ui->hostnameLineEdit->text().toStdString() + ":" + boost::lexical_cast<std::string>(ui->portSpinBox->value());
    }
    else
    {
        filename = ui->filenameLineEdit->displayText().toStdString();
    }

    string db_user = ui->usernameLineEdit->text().toStdString();
    string db_pass = ui->passwordLineEdit->text().toStdString();
    string db_host = ui->hostnameLineEdit->text().toStdString();
    string db_name = ui->databaseNameLineEdit->text().toStdString();
    string db_table = ui->tableNameLineEdit->text().toStdString();
    int db_numIterations = ui->numIterationsSpinBox->value();

    //Define input class (an AisInputSource)
    //STEPX: choose the correct type of input source
    InputType aisInputSource(filename);


    //Define output class (an AisWriter)
    //STEPX: choose the correct type of output source
    OutputType aisWriter(db_user, db_pass, db_host, db_name, db_table, db_numIterations);

    unsigned int progress = 1;
    if(!aisWriter.isReady())
    {
        aisDebug("AisWriter is not ready");
         ui->statusbar->showMessage("Error parsing AIS...", 5000);
        return -1;
    }

    while(aisInputSource.isReady())
    {
        updateProgress(progress++, statusMessage);
        //load the next sentence from the AIS input to the parser
        //STEPX: choose the correct type of sentence parser
        AisSentenceParserType aisSentenceParser(aisInputSource.getNextSentence());
        AisMessageParser aisMessageParser;

        if(aisSentenceParser.isMessageValid())
        {
            //This check is to make sure that if the first sentence of the message
            //was bad we won't read the second sentence and parse it as a new message
            if(aisSentenceParser.getSentenceNumber()==1)
            {
                aisMessageParser.addData(aisSentenceParser.getData());
                //if the current sentence is part of a multipart message
                //grab the next message until you have them all, or message is invalid
                try
                {
                    while(aisSentenceParser.getSentenceNumber() < aisSentenceParser.getNumberOfSentences())
                    {
                        aisSentenceParser.setSentence(aisInputSource.getNextSentence());
                        if(aisSentenceParser.isMessageValid()){
                            aisMessageParser.addData(aisSentenceParser.getData());
                        }
                        else
                        {
                            //aisDebug("Invalid multipart message:\n" + aisSentenceParser.getCurrentSentence());
                            throw std::runtime_error("Invalid multipart message");
                        }
                    }

                    AisMessage aisMessage = aisMessageParser.parseMessage();
                    //add time from ais sentence to the ais message
                    aisMessage.setDATETIME(aisSentenceParser.getTimestamp());
                    //add streamid from ais sentence to the ais message
                    aisMessage.setSTREAMID(aisSentenceParser.getStreamId());

                    aisWriter.writeEntry(aisMessage);
                }
                catch(exception &e)
                {
                    cerr << e.what() << endl;
                }
            }
            else
            {
                aisDebug("First sentence of message was invalid/not receieved.\nSkipping the rest of the sentences of this message");
                continue;
            }
        }
        else
        {
            //aisDebug("Invalid message:\n" + aisSentenceParser.getCurrentSentence());
        }
    }
    ui->statusbar->showMessage("Done parsing AIS...");
    return 0;
}
