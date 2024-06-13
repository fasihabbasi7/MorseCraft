#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <unordered_map>
#include <QPushButton>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <list>
#include <QDebug>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    morseTreeRoot(nullptr)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::on_pushButton_2_clicked);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::on_pushButton_3_clicked);

    initMorseMap(); // Call the function to initialize Morse map
    initializeReverseMorseCodeMap(); // Call the function to initialize reverse Morse map
    buildMorseTree(); // Build Morse code tree
    loadHistory(); // Load history from file

    qDebug() << "MainWindow initialized";
}

MainWindow::~MainWindow()
{
    saveHistory(); // Save history to file
    delete ui;
    qDebug() << "MainWindow destroyed";
}

void MainWindow::on_pushButton_clicked()
{
    QString plainText = ui->lineEdit->text();
    ui->listWidget->addItem(plainText);
    historyList.push_back(plainText); // Push to history linked list
    logFrequency(); // Log character frequency
}

void MainWindow::on_pushButton_2_clicked()
{
    QString plainText = ui->lineEdit->text();
    if (plainText.isEmpty()) {
        return;
    }
    conversionQueue.push(plainText); // Add to conversion queue
    processQueue(); // Process the queue
    ui->lineEdit->clear(); // Clear the input field after converting
}

void MainWindow::on_pushButton_3_clicked()
{
    QString morseCode = ui->lineEdit_2->text();
    QString plainText = morseToText(morseCode);
    ui->listWidget_2->addItem(plainText);
    historyList.push_back(morseCode); // Push to history linked list
}

void MainWindow::initMorseMap()
{
    morseMap['A'] = ".-";
    morseMap['B'] = "-...";
    morseMap['C'] = "-.-.";
    morseMap['D'] = "-..";
    morseMap['E'] = ".";
    morseMap['F'] = "..-.";
    morseMap['G'] = "--.";
    morseMap['H'] = "....";
    morseMap['I'] = "..";
    morseMap['J'] = ".---";
    morseMap['K'] = "-.-";
    morseMap['L'] = ".-..";
    morseMap['M'] = "--";
    morseMap['N'] = "-.";
    morseMap['O'] = "---";
    morseMap['P'] = ".--.";
    morseMap['Q'] = "--.-";
    morseMap['R'] = ".-.";
    morseMap['S'] = "...";
    morseMap['T'] = "-";
    morseMap['U'] = "..-";
    morseMap['V'] = "...-";
    morseMap['W'] = ".--";
    morseMap['X'] = "-..-";
    morseMap['Y'] = "-.--";
    morseMap['Z'] = "--..";
    morseMap['0'] = "-----";
    morseMap['1'] = ".----";
    morseMap['2'] = "..---";
    morseMap['3'] = "...--";
    morseMap['4'] = "....-";
    morseMap['5'] = ".....";
    morseMap['6'] = "-....";
    morseMap['7'] = "--...";
    morseMap['8'] = "---..";
    morseMap['9'] = "----.";
    morseMap[' '] = "/"; // Space between words
}

void MainWindow::initializeReverseMorseCodeMap()
{
    for (auto& pair : morseMap) {
        reverseMorseMap[pair.second] = pair.first;
    }
}

void MainWindow::buildMorseTree()
{
    morseTreeRoot = new TreeNode("");
    for (auto& pair : morseMap) {
        TreeNode* currentNode = morseTreeRoot;
        QString code = pair.second;
        for (QChar ch : code) {
            if (ch == '.') {
                if (!currentNode->left) {
                    currentNode->left = new TreeNode("");
                }
                currentNode = currentNode->left;
            } else if (ch == '-') {
                if (!currentNode->right) {
                    currentNode->right = new TreeNode("");
                }
                currentNode = currentNode->right;
            }
        }
        currentNode->value = pair.first;
    }
}

void MainWindow::traverseMorseTree(TreeNode* node, QString &result)
{
    if (node) {
        if (!node->value.isEmpty()) {
            result += node->value + " ";
        }
        traverseMorseTree(node->left, result);
        traverseMorseTree(node->right, result);
    }
}

QString MainWindow::textToMorse(const QString& text)
{
    QString morseCode;
    for (QChar ch : text.toUpper()) {
        auto it = morseMap.find(ch);
        if (it != morseMap.end()) {
            morseCode += it->second + " ";
            uniqueMorseSet.insert(it->second); // Add to unique Morse set
        } else {
            morseCode += "/ "; // Space for unknown characters
        }
    }
    return morseCode.trimmed(); // Remove trailing space
}

QString MainWindow::morseToText(const QString& morseCode)
{
    QString plainText;
    QStringList tokens = morseCode.split(" ");
    for (const QString& token : tokens) {
        auto it = reverseMorseMap.find(token);
        if (it != reverseMorseMap.end()) {
            plainText += it->second;
        } else if (token == "/") {
            plainText += " "; // Space between words
        } else {
            plainText += QChar::Null; // Unknown Morse code
        }
    }
    return plainText;
}

void MainWindow::processQueue()
{
    while (!conversionQueue.empty()) {
        QString plainText = conversionQueue.front();
        conversionQueue.pop();
        QString morseCode = textToMorse(plainText);
        ui->listWidget->addItem(morseCode);
        historyList.push_back(morseCode); // Push to history linked list
    }
}

void MainWindow::logFrequency()
{
    frequencyMap.clear();
    QString plainText = ui->lineEdit->text();
    for (QChar ch : plainText.toUpper()) {
        if (morseMap.find(ch) != morseMap.end()) {
            frequencyMap[ch]++;
        }
    }
    for (const auto& pair : frequencyMap) {
        qDebug() << pair.first << ":" << pair.second;
    }
}

void MainWindow::saveHistory()
{
    QFile file("history.txt");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        for (const QString& item : historyList) {
            out << item << "\n";
        }
        file.close();
    }
}

void MainWindow::loadHistory()
{
    QFile file("history.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            historyList.push_back(line);
        }
        file.close();
    }
}

void MainWindow::printHistory()
{
    for (const QString& item : historyList) {
        qDebug() << item;
    }
}
