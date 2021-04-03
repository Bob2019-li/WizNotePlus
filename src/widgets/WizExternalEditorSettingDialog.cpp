﻿#include "WizExternalEditorSettingDialog.h"
#include "ui_WizExternalEditorInfoDialog.h"
#include "ui_WizExternalEditorSettingDialog.h"

#include <QSettings>
#include <QDebug>
#include <QDir>
#include <QMap>
#include <QStringList>
#include <QPushButton>
#include <QVariant>
#include <QTableWidgetItem>
#include <QIntValidator>
#include <QRegExpValidator>
#include <QLineEdit>
#include <QCheckBox>
#include <QRegExp>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QKeySequenceEdit>

#include "utils/WizPathResolve.h"

//-------------------------------------------------------------------
// Editor Info Adding Dialog
//-------------------------------------------------------------------

WizExternalEditorInfoDialog::WizExternalEditorInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WizExternalEditorInfoDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Editor Information"));
    m_editProgram = ui->editProgram;
    m_editName = ui->editName;
    m_editArguments = ui->editArguments;
    m_checkTextEditor = ui->checkTextEditor;
    m_checkUTF8 = ui->checkUTF8;
    m_editOpenShortCut = ui->editOpenShortCut;
    m_clearOpenShortCut = ui->clearOpenShortCut;
    // Program File line
    connect(ui->pushBrowse, SIGNAL(clicked()), this, SLOT(setSelectedProgramFile()));
    connect(m_clearOpenShortCut, &QPushButton::clicked, m_editOpenShortCut, &QKeySequenceEdit::clear);
    // Arguments line
    m_editArguments->setText(QString("%1"));

}

WizExternalEditorInfoDialog::WizExternalEditorInfoDialog(int dataRow, SettingMap data, QWidget *parent)
    : WizExternalEditorInfoDialog(parent)
{
    m_dataRow = dataRow;
    m_isEditing = true;
    initForm(data);
}

WizExternalEditorInfoDialog::~WizExternalEditorInfoDialog()
{
    delete ui;
}

void WizExternalEditorInfoDialog::initForm(SettingMap& data)
{
    m_editProgram->setText(data["ProgramFile"]);
    m_editName->setText(data["Name"]);
    m_editArguments->setText(data["Arguments"]);
    m_editOpenShortCut->setKeySequence(QKeySequence::fromString(data["OpenShortCut"]));
    if (data["TextEditor"].toInt() != 0)
        m_checkTextEditor->setCheckState(Qt::Checked);
    if (data["UTF8Encoding"].toInt() != 0)
        m_checkUTF8->setCheckState(Qt::Checked);
    
}

void WizExternalEditorInfoDialog::accept()
{
    // Check program file existance
    QFileInfo program(m_editProgram->text());
    if (!program.exists())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Please specify a valid program file."));
        msgBox.exec();
        m_editProgram->setFocus();
        return;
    }
    // Check name existance
    QString name(m_editName->text());
    if (name.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Please specify a Name for this editor."));
        msgBox.exec();
        m_editName->setFocus();
        return;
    }
    // send signal and data
    SettingMap data;
    data["Name"] = m_editName->text();
    data["ProgramFile"] = m_editProgram->text();
    data["Arguments"] = m_editArguments->text();
    data["TextEditor"] = QString::number(static_cast<int>(m_checkTextEditor->checkState()));
    data["UTF8Encoding"] = QString::number(static_cast<int>(m_checkUTF8->checkState()));
    data["OpenShortCut"] = m_editOpenShortCut->keySequence().toString();

    if (m_isEditing) {
        emit dataEdited(m_dataRow, data);
    } else {
        emit dataAdded(data);
    }

    QDialog::accept();
}

void WizExternalEditorInfoDialog::setSelectedProgramFile()
{
    QString programFilePath = QFileDialog::getOpenFileName(this,
                                    tr("Select a program file."), QString("/home"),
                                                           "", 0, 0);
    m_editProgram->setText(programFilePath);

}

//-------------------------------------------------------------------
// External Editor Setting Dialog
//-------------------------------------------------------------------

WizExternalEditorSettingDialog::WizExternalEditorSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WizExternalEditorSettingDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("External Editor Settings"));
    m_extEditorTable = ui->listEditor;
    //
    m_extEditorTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_extEditorTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    //
    m_extEditorSetting = new QSettings(
                Utils::WizPathResolve::dataStorePath() + "externalEditor.ini", QSettings::IniFormat);
    loadDataFromIni(m_extEditorSetting);
    // Ui had already connect this name-type slot function
    // if connect again,
    //connect(ui->btnAddEditor, SIGNAL(clicked()), this, SLOT(on_btnAddEditor_clicked()));
    //connect(ui->btnDeleteEditor, SIGNAL(clicked()), this, SLOT(on_btnDeleteEditor_clicked()));
    //connect(ui->btnEditSetting, SIGNAL(clicked()), this, SLOT(on_btnEditSetting_clicked()));
}

WizExternalEditorSettingDialog::~WizExternalEditorSettingDialog()
{
    delete ui;
}

void WizExternalEditorSettingDialog::loadDataFromIni(QSettings* settings)
{
    QStringList groups = settings->childGroups();
    m_extEditorTable->setRowCount(groups.length());
    int row = 0;
    for (QString& editorIndex : groups) {
        settings->beginGroup(editorIndex);
        QString Name = settings->value("Name").toString();
        QString ProgramFile = settings->value("ProgramFile").toString();
        QString Arguments = settings->value("Arguments", "%1").toString();
        QString TextEditor = settings->value("TextEditor", 0).toString();
        QString UTF8Encoding = settings->value("UTF8Encoding", 0).toString();
        QString OpenShortCut = settings->value("OpenShortCut", "").toString();

        m_extEditorTable->setItem(row, 0, new QTableWidgetItem(Name));
        m_extEditorTable->setItem(row, 1, new QTableWidgetItem(ProgramFile));
        m_extEditorTable->setItem(row, 2, new QTableWidgetItem(Arguments));
        m_extEditorTable->setItem(row, 3, new QTableWidgetItem(TextEditor));
        m_extEditorTable->setItem(row, 4, new QTableWidgetItem(UTF8Encoding));
        m_extEditorTable->setItem(row, 5, new QTableWidgetItem(OpenShortCut));

        ++row;

        settings->endGroup();
    }
}

void WizExternalEditorSettingDialog::writeDataToIni(QSettings* settings)
{
    settings->clear();
    int rowLength = m_extEditorTable->rowCount();
    for (int i = 0; i < rowLength; ++i)
    {
        QString Name = m_extEditorTable->item(i, 0)->text();
        QString ProgramFile = m_extEditorTable->item(i, 1)->text();
        QString Arguments = m_extEditorTable->item(i, 2)->text();
        QString TextEditor = m_extEditorTable->item(i, 3)->text();
        QString UTF8Encoding = m_extEditorTable->item(i, 4)->text();
        QString OpenShortCut = m_extEditorTable->item(i, 5)->text();
        //
        settings->beginGroup("Editor_" + QString::number(i));
        settings->setValue("Name", Name);
        settings->setValue("ProgramFile", ProgramFile);
        settings->setValue("Arguments", Arguments);
        settings->setValue("TextEditor", TextEditor);
        settings->setValue("UTF8Encoding", UTF8Encoding);
        settings->setValue("OpenShortCut", OpenShortCut);
        settings->endGroup();
    }
    settings->sync();
    emit settingChanged();
}

void WizExternalEditorSettingDialog::on_btnAddEditor_clicked()
{
    WizExternalEditorInfoDialog* infoDialog = new WizExternalEditorInfoDialog(this);
    infoDialog->setAttribute(Qt::WA_DeleteOnClose);
    //
    connect(infoDialog, SIGNAL(dataAdded(SettingMap&)), this, SLOT(addEditor(SettingMap&)));
    //
    infoDialog->exec();
}

void WizExternalEditorSettingDialog::modifyRowContent(int row, SettingMap& data)
{
    m_extEditorTable->setItem(row, 0, new QTableWidgetItem(data["Name"]));
    m_extEditorTable->setItem(row, 1, new QTableWidgetItem(data["ProgramFile"]));
    m_extEditorTable->setItem(row, 2, new QTableWidgetItem(data["Arguments"]));
    m_extEditorTable->setItem(row, 3, new QTableWidgetItem(data["TextEditor"]));
    m_extEditorTable->setItem(row, 4, new QTableWidgetItem(data["UTF8Encoding"]));
    m_extEditorTable->setItem(row, 5, new QTableWidgetItem(data["OpenShortCut"]));
}

void WizExternalEditorSettingDialog::addEditor(SettingMap& data)
{
    int rowLength = 0;
    rowLength = m_extEditorTable->rowCount();
    m_extEditorTable->setRowCount(rowLength + 1);
    //
    modifyRowContent(rowLength, data);
}

void WizExternalEditorSettingDialog::on_btnDeleteEditor_clicked()
{
    // Get Selection
    QItemSelectionModel* selection = m_extEditorTable->selectionModel();
    if (selection->hasSelection())
    {
        for (QModelIndex& row : selection->selectedRows())
        {
            m_extEditorTable->removeRow(row.row());
        }
    }
}

void WizExternalEditorSettingDialog::on_btnEditSetting_clicked()
{
    // Get Selection and data
    QModelIndexList selectedRows = m_extEditorTable->selectionModel()->selectedRows();
    if (selectedRows.length() == 0 )
        return;
    int row = selectedRows.first().row();
    SettingMap data;
    data["Name"] = m_extEditorTable->item(row, 0)->text();
    data["ProgramFile"] = m_extEditorTable->item(row, 1)->text();
    data["Arguments"] = m_extEditorTable->item(row, 2)->text();
    data["TextEditor"] = m_extEditorTable->item(row, 3)->text();
    data["UTF8Encoding"] = m_extEditorTable->item(row, 4)->text();
    data["OpenShortCut"] = m_extEditorTable->item(row, 5)->text();
    // Open Dialog
    WizExternalEditorInfoDialog* infoDialog = new WizExternalEditorInfoDialog(row, data, this);
    infoDialog->setAttribute(Qt::WA_DeleteOnClose);
    //
    connect(infoDialog, SIGNAL(dataEdited(int, SettingMap&)), this, SLOT(modifyEditor(int, SettingMap&)));
    //
    infoDialog->exec();

}

void WizExternalEditorSettingDialog::modifyEditor(int row, SettingMap& data)
{
    modifyRowContent(row, data);
}

void WizExternalEditorSettingDialog::accept()
{
    // Write Setting
    writeDataToIni(m_extEditorSetting);
    //
    QDialog::accept();
}
