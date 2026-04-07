#ifndef FINALDATASTRUCTUREPROJECT_H
#define FINALDATASTRUCTUREPROJECT_H

#include <QMainWindow>
#include <QTableWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QTimer>
#include <QVector>
#include <QPointF>
#include <QWidget>  // Added missing include
#include <QLabel>   // Added missing include

const int INF = 1000000000;

class FINALdataStructurePROJECT : public QMainWindow
{
    Q_OBJECT

public:
    explicit FINALdataStructurePROJECT(QWidget* parent = nullptr);
    ~FINALdataStructurePROJECT() = default;  // Add destructor

private slots:
    void runDijkstra();
    void animateStep();
    void setNumberOfNodes();

private:
    // ===== UI =====
    QTableWidget* matrixTable;
    QLineEdit* sourceEdit;
    QPushButton* runButton;
    QTextEdit* logBox;
    QGraphicsView* view;
    QGraphicsScene* scene;
    QTableWidget* stepsTable;
    QLineEdit* targetEdit;
    QLineEdit* nodesEdit;
    QPushButton* setNodesButton;  // Added missing member
    // ===== UI setup =====
    void setupUI();
    void initializeDynamicGraph();

    // ===== Matrix helpers =====
    void generateRandomGraph();
    void clearMatrix();
    void showTableContextMenu(const QPoint& pos);

    // ===== Export =====
    void exportStepsToFile();

    // ===== Graph data =====
    int n;
    int source;
    int targetNode;
    QVector<QVector<int>> graph;
    QVector<int> dist;
    QVector<int> parent;
    QVector<bool> visited;

    // ===== Drawing =====
    QVector<QPointF> positions;

    // ===== Animation =====
    QVector<int> visitOrder;
    int animIndex;
    QTimer* timer;

    // ===== Steps table =====
    int iterationCount;
    void addStepToTable(int currentNode);

    // ===== Core logic =====
    void readGraphFromTable();
    void dijkstraAlgorithm();

    // ===== Visualization =====
    void drawGraph(int current = -1);
    void drawPathTo(int target);

    // ===== Helper =====
    void log(const QString& text);

 
};

#endif