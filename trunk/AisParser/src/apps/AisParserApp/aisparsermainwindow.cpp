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
#include <AisParserTemplates.h>

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
	settings.setValue("input_file_type", ui->inputTypeComboBox->currentIndex());
	settings.setValue("input_file", ui->filenameLineEdit->text());
	settings.setValue("tcp_hostname", ui->inputHostnameLineEdit->text());
	settings.setValue("tcp_port", ui->portSpinBox->value());
	settings.setValue("output_type", ui->outputTypeComboBox->currentIndex());
	settings.setValue("messages_per_file", ui->messagesPerFileSpinBox->value());
	settings.setValue("output_dir", ui->outputDirectoryLineEdit->text());
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
	ui->inputTypeComboBox->setCurrentIndex(settings.value("input_file_type",0).toInt());
	ui->filenameLineEdit->setText(settings.value("input_file", "").toString());
	ui->inputHostnameLineEdit->setText(settings.value("tcp_hostname", "localhost").toString());
	ui->portSpinBox->setValue(settings.value("tcp_port", 10000).toInt());
	ui->outputTypeComboBox->setCurrentIndex(settings.value("output_type", 0).toInt());
	ui->messagesPerFileSpinBox->setValue(settings.value("messages_per_file", 0).toInt());
	ui->outputDirectoryLineEdit->setText(settings.value("output_dir", "").toString());
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

	std::string filename = ui->filenameLineEdit->text().toStdString();		
	unsigned int messagesPerFile = ui->messagesPerFileSpinBox->value();

	std::string db_username = ui->usernameLineEdit->text().toStdString();
	std::string db_password = ui->passwordLineEdit->text().toStdString();
	std::string db_hostname = ui->hostnameLineEdit->text().toStdString();
	std::string db_name = ui->databaseNameLineEdit->text().toStdString();
	std::string db_table = ui->tableNameLineEdit->text().toStdString();
	std::string db_numIterations = ui->numIterationsSpinBox->text().toStdString();

    if( inputChoice == "Satellite AIS Log")
    {
		AisFlatFileInputSource aisInputSource(filename);

        if(outputChoice == "Tab-Seperated Files")
        {
            flatfileParser<AisTsvWriter, AisSatSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
		else if(outputChoice == "Comma-Seperated Files")
        {
			flatfileParser<AisCsvWriter, AisSatSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
		else if(outputChoice == "PostgreSQL Database")
        {
            databaseParser<AisPostgreSqlDatabaseWriter, AisSatSentenceParser>(aisInputSource, db_username, db_password, db_hostname, db_name, db_table, db_numIterations, "");
        }
        else if(outputChoice == "MySql Database")
        {
            databaseParser<AisMySqlDatabaseWriter, AisSatSentenceParser>(aisInputSource, db_username, db_password, db_hostname, db_name, db_table, db_numIterations, "");
        }
        else if(outputChoice == "Oracle Database")
        {
            databaseParser<AisDatabaseWriter, AisSatSentenceParser>(aisInputSource, db_username, db_password, db_hostname, db_name, db_table, db_numIterations, "");
        }
        else if(outputChoice == "KML Placemarks")
        {
			flatfileParser<AisKmlPlacemarkWriter, AisSatSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
        else if(outputChoice == "KML Tracks")
        {
            trackParser<AisKmlTrackWriter, AisSatSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
        else if(outputChoice == "ESRI Shapefile Points")
        {
			flatfileParser<AisShapefilePointWriter, AisSatSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
        else if(outputChoice == "ESRI Shapefile Tracks")
        {
            trackParser<AisShapefileTrackWriter, AisSatSentenceParser>(aisInputSource, filename, messagesPerFile);
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
		AisFlatFileInputSource aisInputSource(filename);

		if(outputChoice == "Tab-Seperated Files")
        {
            flatfileParser<AisTsvWriter, AisMsisSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
		else if(outputChoice == "Comma-Seperated Files")
        {
			flatfileParser<AisCsvWriter, AisMsisSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
		else if(outputChoice == "PostgreSQL Database")
        {
            databaseParser<AisPostgreSqlDatabaseWriter, AisMsisSentenceParser>(aisInputSource, db_username, db_password, db_hostname, db_name, db_table, db_numIterations, "");
        }
        else if(outputChoice == "MySql Database")
        {
            databaseParser<AisMySqlDatabaseWriter, AisMsisSentenceParser>(aisInputSource, db_username, db_password, db_hostname, db_name, db_table, db_numIterations, "");
        }
        else if(outputChoice == "Oracle Database")
        {
            databaseParser<AisDatabaseWriter, AisMsisSentenceParser>(aisInputSource, db_username, db_password, db_hostname, db_name, db_table, db_numIterations, "");
        }
        else if(outputChoice == "KML Placemarks")
        {
			flatfileParser<AisKmlPlacemarkWriter, AisMsisSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
        else if(outputChoice == "KML Tracks")
        {
            trackParser<AisKmlTrackWriter, AisMsisSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
        else if(outputChoice == "ESRI Shapefile Points")
        {
			flatfileParser<AisShapefilePointWriter, AisMsisSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
        else if(outputChoice == "ESRI Shapefile Tracks")
        {
            trackParser<AisShapefileTrackWriter, AisMsisSentenceParser>(aisInputSource, filename, messagesPerFile);
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
		filename = "";
		AisTcpStreamInputSource aisInputSource(ui->inputHostnameLineEdit->text().toStdString(), ui->portSpinBox->text().toStdString());

        if(outputChoice == "Tab-Seperated Files")
        {
			flatfileParser<AisTsvWriter, AisSatSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
		else if(outputChoice == "Comma-Seperated Files")
        {
			flatfileParser<AisCsvWriter, AisSatSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
		else if(outputChoice == "PostgreSQL Database")
        {
            databaseParser<AisPostgreSqlDatabaseWriter, AisSatSentenceParser>(aisInputSource, db_username, db_password, db_hostname, db_name, db_table, db_numIterations, "");
        }
        else if(outputChoice == "MySql Database")
        {
            databaseParser<AisMySqlDatabaseWriter, AisSatSentenceParser>(aisInputSource, db_username, db_password, db_hostname, db_name, db_table, db_numIterations, "");
        }
        else if(outputChoice == "Oracle Database")
        {
            databaseParser<AisDatabaseWriter, AisSatSentenceParser>(aisInputSource, db_username, db_password, db_hostname, db_name, db_table, db_numIterations, "");
        }
        else if(outputChoice == "KML Placemarks")
        {
			flatfileParser<AisKmlPlacemarkWriter, AisSatSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
        else if(outputChoice == "KML Tracks")
        {
            trackParser<AisKmlTrackWriter, AisSatSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
        else if(outputChoice == "ESRI Shapefile Points")
        {
			flatfileParser<AisShapefilePointWriter, AisSatSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
        else if(outputChoice == "ESRI Shapefile Tracks")
        {
            trackParser<AisShapefileTrackWriter, AisSatSentenceParser>(aisInputSource, filename, messagesPerFile);
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
		filename = "";
		AisTcpStreamInputSource aisInputSource(ui->inputHostnameLineEdit->text().toStdString(), ui->portSpinBox->text().toStdString());

        if(outputChoice == "Tab-Seperated Files")
        {
			flatfileParser<AisTsvWriter, AisMsisSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
		else if(outputChoice == "Comma-Seperated Files")
        {
			flatfileParser<AisCsvWriter, AisMsisSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
		else if(outputChoice == "PostgreSQL Database")
        {
            databaseParser<AisPostgreSqlDatabaseWriter, AisMsisSentenceParser>(aisInputSource, db_username, db_password, db_hostname, db_name, db_table, db_numIterations, "");
        }
        else if(outputChoice == "MySql Database")
        {
            databaseParser<AisMySqlDatabaseWriter, AisMsisSentenceParser>(aisInputSource, db_username, db_password, db_hostname, db_name, db_table, db_numIterations, "");
        }
        else if(outputChoice == "Oracle Database")
        {
            databaseParser<AisDatabaseWriter, AisMsisSentenceParser>(aisInputSource, db_username, db_password, db_hostname, db_name, db_table, db_numIterations, "");
        }
        else if(outputChoice == "KML Placemarks")
        {
			flatfileParser<AisKmlPlacemarkWriter, AisMsisSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
        else if(outputChoice == "KML Tracks")
        {
            trackParser<AisKmlTrackWriter, AisMsisSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
        else if(outputChoice == "ESRI Shapefile Points")
        {
			flatfileParser<AisShapefilePointWriter, AisMsisSentenceParser>(aisInputSource, filename, messagesPerFile);
        }
        else if(outputChoice == "ESRI Shapefile Tracks")
        {
            trackParser<AisShapefileTrackWriter, AisMsisSentenceParser>(aisInputSource, filename, messagesPerFile);
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

