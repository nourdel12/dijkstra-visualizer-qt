#define _USE_MATH_DEFINES
#include "FINALdataStructurePROJECT.h"
#include "PriorityQueue.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QLabel>
#include <QMessageBox>
#include <QSpacerItem>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <QMenu>
#include <QAction>
#include <QIntValidator>
#include <QTextCursor>
#include <QStringList>

// ================= Constructor =================
FINALdataStructurePROJECT::FINALdataStructurePROJECT(QWidget* parent)
    : QMainWindow(parent)
    , matrixTable(nullptr)
    , sourceEdit(nullptr)
    , runButton(nullptr)
    , logBox(nullptr)
    , view(nullptr)
    , scene(nullptr)
    , stepsTable(nullptr)
    , targetEdit(nullptr)
    , nodesEdit(nullptr)
    , setNodesButton(nullptr)
    , n(5)  // Start with 5 nodes
    , source(0)
    , targetNode(2)
    , animIndex(0)
    , iterationCount(0)
    , timer(nullptr)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    setupUI();
    initializeDynamicGraph();

    setWindowTitle("Dijkstra Algorithm Visualizer - FINAL Data Structure Project");
    resize(1100, 850);

    timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, &QTimer::timeout, this, &FINALdataStructurePROJECT::animateStep);
    connect(runButton, &QPushButton::clicked, this, &FINALdataStructurePROJECT::runDijkstra);

    log("✓ Application started successfully");
    log(QString("✓ Dynamic graph initialized with %1 nodes").arg(n));
    log("✓ User can resize graph at any time");
}

void FINALdataStructurePROJECT::setupUI()
{
    // -------- Adjacency Matrix --------
    matrixTable = new QTableWidget(5, 5, this);  // Start with 5x5
    matrixTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    matrixTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Allow user to resize rows/columns
    matrixTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    matrixTable->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    // Allow user to add/remove rows/columns via context menu
    matrixTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(matrixTable, &QTableWidget::customContextMenuRequested, [this](const QPoint& pos) {
        showTableContextMenu(pos);
        });

    // -------- Controls --------
    QWidget* controlWidget = new QWidget(this);
    QHBoxLayout* controlLayout = new QHBoxLayout(controlWidget);
    controlLayout->setContentsMargins(0, 0, 0, 0);

    // Node count control
    QLabel* nodesLabel = new QLabel("Nodes:", this);
    nodesEdit = new QLineEdit(this);
    nodesEdit->setText("5");
    nodesEdit->setMaximumWidth(60);
    nodesEdit->setValidator(new QIntValidator(1, 20, this));

    setNodesButton = new QPushButton("Apply", this);
    setNodesButton->setMaximumWidth(80);

    QLabel* sourceLabel = new QLabel("Source:", this);
    sourceEdit = new QLineEdit(this);
    sourceEdit->setText("0");
    sourceEdit->setMaximumWidth(60);

    QLabel* targetLabel = new QLabel("Target:", this);
    targetEdit = new QLineEdit(this);
    targetEdit->setText("2");
    targetEdit->setMaximumWidth(60);

    runButton = new QPushButton("Run Dijkstra", this);
    runButton->setMaximumWidth(120);

    QPushButton* randomButton = new QPushButton("Random Graph", this);
    randomButton->setMaximumWidth(120);
    connect(randomButton, &QPushButton::clicked, this, [this]() {
        generateRandomGraph();
        });

    // Layout for controls
    controlLayout->addWidget(nodesLabel);
    controlLayout->addWidget(nodesEdit);
    controlLayout->addWidget(setNodesButton);
    controlLayout->addSpacing(20);
    controlLayout->addWidget(sourceLabel);
    controlLayout->addWidget(sourceEdit);
    controlLayout->addSpacing(20);
    controlLayout->addWidget(targetLabel);
    controlLayout->addWidget(targetEdit);
    controlLayout->addSpacing(20);
    controlLayout->addWidget(runButton);
    controlLayout->addSpacing(10);
    controlLayout->addWidget(randomButton);
    controlLayout->addStretch();

    connect(setNodesButton, &QPushButton::clicked, this, &FINALdataStructurePROJECT::setNumberOfNodes);

    // -------- Log Box --------
    logBox = new QTextEdit(this);
    logBox->setReadOnly(true);
    logBox->setMinimumHeight(220);
    logBox->setFont(QFont("Consolas", 9));

    // -------- Steps Table --------
    stepsTable = new QTableWidget(this);
    stepsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    stepsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    stepsTable->setMinimumHeight(120);
    stepsTable->setAlternatingRowColors(true);

    // -------- Graphics View --------
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    view->setMinimumHeight(350);
    view->setRenderHint(QPainter::Antialiasing);
    view->setSceneRect(0, 0, 800, 500);

    // -------- Main Layout --------
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(5);

    // Matrix section
    QHBoxLayout* matrixHeaderLayout = new QHBoxLayout();
    matrixHeaderLayout->addWidget(new QLabel("Adjacency Matrix (INF = no connection):"));
    QPushButton* clearMatrixButton = new QPushButton("Clear Matrix", this);
    clearMatrixButton->setMaximumWidth(100);
    connect(clearMatrixButton, &QPushButton::clicked, this, [this]() {
        clearMatrix();
        });
    matrixHeaderLayout->addWidget(clearMatrixButton);
    matrixHeaderLayout->addStretch();

    mainLayout->addLayout(matrixHeaderLayout);
    mainLayout->addWidget(matrixTable);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(controlWidget);
    mainLayout->addSpacing(10);

    // Visualization section
    QHBoxLayout* visHeaderLayout = new QHBoxLayout();
    visHeaderLayout->addWidget(new QLabel("Graph Visualization:"));
    QPushButton* resetViewButton = new QPushButton("Reset View", this);
    resetViewButton->setMaximumWidth(100);
    connect(resetViewButton, &QPushButton::clicked, this, [this]() {
        view->resetTransform();
        view->centerOn(scene->itemsBoundingRect().center());
        });
    visHeaderLayout->addWidget(resetViewButton);
    visHeaderLayout->addStretch();

    mainLayout->addLayout(visHeaderLayout);
    mainLayout->addWidget(view);
    mainLayout->addSpacing(10);

    // Steps table
    QHBoxLayout* stepsHeaderLayout = new QHBoxLayout();
    stepsHeaderLayout->addWidget(new QLabel("Dijkstra Steps:"));
    QPushButton* exportButton = new QPushButton("Export Steps", this);
    exportButton->setMaximumWidth(150);
    connect(exportButton, &QPushButton::clicked, this, [this]() {
        exportStepsToFile();
        });
    stepsHeaderLayout->addWidget(exportButton);
    stepsHeaderLayout->addStretch();

    mainLayout->addLayout(stepsHeaderLayout);
    mainLayout->addWidget(stepsTable);
    mainLayout->addSpacing(10);

    // Log section
    QHBoxLayout* logHeaderLayout = new QHBoxLayout();
    logHeaderLayout->addWidget(new QLabel("Execution Log:"));
    QPushButton* clearLogButton = new QPushButton("Clear Log", this);
    clearLogButton->setMaximumWidth(100);
    connect(clearLogButton, &QPushButton::clicked, this, [this]() {
        logBox->clear();
        });
    logHeaderLayout->addWidget(clearLogButton);
    logHeaderLayout->addStretch();

    mainLayout->addLayout(logHeaderLayout);
    mainLayout->addWidget(logBox);

    setCentralWidget(centralWidget);
}

void FINALdataStructurePROJECT::initializeDynamicGraph()
{
    // Start with 5 nodes
    n = 5;

    // Clear and setup matrix table
    matrixTable->setRowCount(n);
    matrixTable->setColumnCount(n);

    // Setup headers
    for (int i = 0; i < n; i++) {
        QTableWidgetItem* headerItem = new QTableWidgetItem(QString::number(i));
        headerItem->setTextAlignment(Qt::AlignCenter);
        matrixTable->setHorizontalHeaderItem(i, headerItem);
        matrixTable->setVerticalHeaderItem(i, new QTableWidgetItem(QString::number(i)));
    }

    // Fill with random weighted graph
    generateRandomGraph();

    // Set source and target to reasonable values
    source = 0;
    targetNode = n - 1;
    sourceEdit->setText(QString::number(source));
    targetEdit->setText(QString::number(targetNode));
}

void FINALdataStructurePROJECT::generateRandomGraph()
{
    int currentN = matrixTable->rowCount();

    // Fill matrix with random values
    for (int i = 0; i < currentN; i++) {
        for (int j = 0; j < currentN; j++) {
            QTableWidgetItem* item = matrixTable->item(i, j);
            if (!item) {
                item = new QTableWidgetItem();
                matrixTable->setItem(i, j, item);
            }

            if (i == j) {
                item->setText("0");
                item->setBackground(Qt::lightGray);
            }
            else {
                // 70% chance of having an edge, 30% INF
                if (rand() % 100 < 70) {
                    int weight = rand() % 15 + 1;  // 1-15
                    item->setText(QString::number(weight));
                    item->setBackground(Qt::white);
                }
                else {
                    item->setText("INF");
                    item->setBackground(QColor(255, 230, 230));
                }
            }

            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        }
    }

    // Make roughly symmetric (undirected graph)
    for (int i = 0; i < currentN; i++) {
        for (int j = i + 1; j < currentN; j++) {
            QTableWidgetItem* item1 = matrixTable->item(i, j);
            QTableWidgetItem* item2 = matrixTable->item(j, i);

            if (item1 && item2 && item1->text() != "INF" && rand() % 100 < 80) {
                // Make symmetric (same weight in both directions)
                item2->setText(item1->text());
                item2->setBackground(item1->background());
            }
        }
    }

    log("✓ Generated random weighted graph");
    log("✓ 0 = self, 1-15 = edge weight, INF = no connection");

    // Update graph visualization
    readGraphFromTable();
    drawGraph();
}

void FINALdataStructurePROJECT::clearMatrix()
{
    int currentN = matrixTable->rowCount();

    for (int i = 0; i < currentN; i++) {
        for (int j = 0; j < currentN; j++) {
            QTableWidgetItem* item = matrixTable->item(i, j);
            if (!item) {
                item = new QTableWidgetItem();
                matrixTable->setItem(i, j, item);
            }

            if (i == j) {
                item->setText("0");
            }
            else {
                item->setText("INF");
            }
            item->setBackground(Qt::white);
            item->setTextAlignment(Qt::AlignCenter);
        }
    }

    log("✓ Matrix cleared (diagonal = 0, others = INF)");
}

void FINALdataStructurePROJECT::showTableContextMenu(const QPoint& pos)
{
    QMenu contextMenu("Table Actions", this);

    QAction* addRowAction = new QAction("Add Row", this);
    QAction* addColAction = new QAction("Add Column", this);
    QAction* removeRowAction = new QAction("Remove Row", this);
    QAction* removeColAction = new QAction("Remove Column", this);

    connect(addRowAction, &QAction::triggered, this, [this]() {
        int newRows = matrixTable->rowCount() + 1;
        matrixTable->setRowCount(newRows);

        // Add new row items
        int cols = matrixTable->columnCount();
        int lastRow = newRows - 1;

        for (int col = 0; col < cols; col++) {
            QTableWidgetItem* item = new QTableWidgetItem();
            if (col == lastRow) {
                item->setText("0");  // Diagonal
            }
            else {
                item->setText("INF");
            }
            item->setTextAlignment(Qt::AlignCenter);
            matrixTable->setItem(lastRow, col, item);
        }

        // Update header
        matrixTable->setVerticalHeaderItem(lastRow,
            new QTableWidgetItem(QString::number(lastRow)));

        log(QString("✓ Added row %1").arg(lastRow));
        });

    connect(addColAction, &QAction::triggered, this, [this]() {
        int newCols = matrixTable->columnCount() + 1;
        matrixTable->setColumnCount(newCols);

        // Add new column items
        int rows = matrixTable->rowCount();
        int lastCol = newCols - 1;

        for (int row = 0; row < rows; row++) {
            QTableWidgetItem* item = new QTableWidgetItem();
            if (row == lastCol) {
                item->setText("0");  // Diagonal
            }
            else {
                item->setText("INF");
            }
            item->setTextAlignment(Qt::AlignCenter);
            matrixTable->setItem(row, lastCol, item);
        }

        // Update header
        matrixTable->setHorizontalHeaderItem(lastCol,
            new QTableWidgetItem(QString::number(lastCol)));

        log(QString("✓ Added column %1").arg(lastCol));
        });

    connect(removeRowAction, &QAction::triggered, this, [this]() {
        int currentRows = matrixTable->rowCount();
        if (currentRows > 2) {
            matrixTable->setRowCount(currentRows - 1);
            log(QString("✓ Removed row (now %1 rows)").arg(currentRows - 1));
        }
        else {
            log("✗ Cannot have less than 2 rows");
        }
        });

    connect(removeColAction, &QAction::triggered, this, [this]() {
        int currentCols = matrixTable->columnCount();
        if (currentCols > 2) {
            matrixTable->setColumnCount(currentCols - 1);
            log(QString("✓ Removed column (now %1 columns)").arg(currentCols - 1));
        }
        else {
            log("✗ Cannot have less than 2 columns");
        }
        });

    contextMenu.addAction(addRowAction);
    contextMenu.addAction(addColAction);
    contextMenu.addSeparator();
    contextMenu.addAction(removeRowAction);
    contextMenu.addAction(removeColAction);

    contextMenu.exec(matrixTable->mapToGlobal(pos));
}

// ================= Read Graph (DYNAMIC VERSION) =================
void FINALdataStructurePROJECT::readGraphFromTable()
{
    int rows = matrixTable->rowCount();
    int cols = matrixTable->columnCount();

    // Ensure square matrix by taking min dimension
    n = std::min(rows, cols);

    if (rows != cols) {
        // Resize to square
        matrixTable->setRowCount(n);
        matrixTable->setColumnCount(n);
        log(QString("⚠ Adjusted to square matrix (%1x%1)").arg(n));
    }

    if (n <= 0) {
        log("✗ Error: Graph has no nodes!");
        return;
    }

    // Update headers
    for (int i = 0; i < n; i++) {
        QTableWidgetItem* header = new QTableWidgetItem(QString::number(i));
        header->setTextAlignment(Qt::AlignCenter);
        matrixTable->setHorizontalHeaderItem(i, header);
        matrixTable->setVerticalHeaderItem(i, new QTableWidgetItem(QString::number(i)));
    }

    // Initialize graph
    graph = QVector<QVector<int>>(n, QVector<int>(n, INF));

    // Read values
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            QTableWidgetItem* item = matrixTable->item(i, j);
            if (!item) {
                item = new QTableWidgetItem("INF");
                item->setTextAlignment(Qt::AlignCenter);
                matrixTable->setItem(i, j, item);
            }

            QString text = item->text().trimmed().toUpper();
            if (i == j) {
                graph[i][j] = 0;
                if (text != "0") {
                    item->setText("0");
                    item->setBackground(Qt::lightGray);
                }
            }
            else if (text.isEmpty() || text == "INF") {
                graph[i][j] = INF;
                item->setBackground(QColor(255, 230, 230));
            }
            else {
                bool ok;
                int value = text.toInt(&ok);
                if (ok && value > 0) {
                    graph[i][j] = value;
                    item->setBackground(Qt::white);
                }
                else {
                    graph[i][j] = INF;
                    item->setText("INF");
                    item->setBackground(QColor(255, 230, 230));
                }
            }
        }
    }
    dist = QVector<int>(n, INF);
    visited = QVector<bool>(n, false);
    parent = QVector<int>(n, -1);

    log(QString("✓ Graph loaded: %1 nodes").arg(n));
}

// ================= Dijkstra Algorithm =================
void FINALdataStructurePROJECT::dijkstraAlgorithm()
{
    // Initialize
    dist = QVector<int>(n, INF);
    parent = QVector<int>(n, -1);
    visited = QVector<bool>(n, false);
    visitOrder.clear();

    dist[source] = 0;

    PriorityQueue<int, int> pq(n);
    for (int i = 0; i < n; i++) {
        pq.push(i, dist[i]);
    }

    // Main loop
    while (!pq.empty()) {
        int u = pq.extractMin().vertex;
        if (visited[u]) continue;

        visited[u] = true;
        visitOrder.push_back(u);

        addStepToTable(u);
        log(QString("Step %1: Visit node %2 (dist=%3)")
            .arg(visitOrder.size()).arg(u).arg(dist[u]));

        // Relax all edges from u
        for (int v = 0; v < n; v++) {
            if (graph[u][v] != INF && !visited[v] &&
                dist[u] + graph[u][v] < dist[v]) {

                int oldDist = dist[v];
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
                pq.decreaseKey(v, dist[v]);

                log(QString("  Relax %1→%2: %3 + %4 = %5 (was %6)")
                    .arg(u).arg(v).arg(dist[u]).arg(graph[u][v])
                    .arg(dist[v]).arg(oldDist == INF ? "INF" : QString::number(oldDist)));
            }
        }
    }
}

// ================= Run Dijkstra =================
void FINALdataStructurePROJECT::runDijkstra()
{
    // Stop any ongoing animation
    if (timer && timer->isActive()) {
        timer->stop();
    }

    // Clear previous results
    logBox->clear();
    stepsTable->clear();
    stepsTable->setRowCount(0);
    iterationCount = 0;

    // Read current graph
    readGraphFromTable();

    if (n <= 0) {
        log("✗ Error: No graph to process");
        return;
    }

    // Validate inputs
    bool ok;
    source = sourceEdit->text().toInt(&ok);
    if (!ok || source < 0 || source >= n) {
        QMessageBox::warning(this, "Invalid Source",
            QString("Source must be 0-%1").arg(n - 1));
        return;
    }

    targetNode = targetEdit->text().toInt(&ok);
    if (!ok || targetNode < 0 || targetNode >= n) {
        QMessageBox::warning(this, "Invalid Target",
            QString("Target must be 0-%1").arg(n - 1));
        return;
    }

    if (source == targetNode) {
        QMessageBox::warning(this, "Same Nodes",
            "Source and target cannot be the same");
        return;
    }

    // Setup steps table
    stepsTable->setColumnCount(n + 2);
    QStringList headers;
    headers << "Iter" << "Current";
    for (int i = 0; i < n; i++) {
        headers << QString("d[%1]").arg(i);
    }
    stepsTable->setHorizontalHeaderLabels(headers);

    // Run algorithm
    log("========== DIJKSTRA ALGORITHM ==========");
    log(QString("Nodes: %1 | Source: %2 | Target: %3")
        .arg(n).arg(source).arg(targetNode));
    log("----------------------------------------");

    dijkstraAlgorithm();

    // Display results
    log("\n========== RESULTS ==========");
    bool reachable = true;
    for (int i = 0; i < n; i++) {
        if (dist[i] == INF) {
            log(QString("Node %1: ∞ (unreachable)").arg(i));
            if (i == targetNode) reachable = false;
        }
        else {
            log(QString("Node %1: %2").arg(i).arg(dist[i]));
        }
    }

    // Show path if reachable
    if (reachable) {
        QVector<int> path;
        for (int v = targetNode; v != -1; v = parent[v]) {
            path.prepend(v);
        }

        QString pathStr;
        int totalCost = 0;
        for (int i = 0; i < path.size(); i++) {
            if (i > 0) {
                pathStr += " → ";
                totalCost += graph[path[i - 1]][path[i]];
            }
            pathStr += QString::number(path[i]);
        }

        log("\n========== SHORTEST PATH ==========");
        log(QString("Path: %1").arg(pathStr));
        log(QString("Total cost: %2").arg(totalCost));
    }
    else {
        log("\n✗ Target node is unreachable!");
    }

    // Start animation
    animIndex = 0;

    if (visitOrder.isEmpty()) {
        log("⚠ No animation steps available");
        drawGraph();
        if (dist[targetNode] != INF)
            drawPathTo(targetNode);
        return;
    }

    timer->start(1200);

}

// ================= Set Number of Nodes =================
void FINALdataStructurePROJECT::setNumberOfNodes()
{
    bool ok;
    int newN = nodesEdit->text().toInt(&ok);

    if (!ok || newN < 2 || newN > 15) {
        QMessageBox::warning(this, "Invalid Value",
            "Number of nodes must be between 2 and 15");
        return;
    }

    // Stop animation
    if (timer && timer->isActive()) {
        timer->stop();
    }

    // Save current values from selected cells
    QList<QPair<QPoint, QString>> savedValues;
    for (int i = 0; i < std::min(n, newN); i++) {
        for (int j = 0; j < std::min(n, newN); j++) {
            QTableWidgetItem* item = matrixTable->item(i, j);
            if (item) {
                savedValues.append(qMakePair(QPoint(i, j), item->text()));
            }
        }
    }

    // Resize table
    matrixTable->setRowCount(newN);
    matrixTable->setColumnCount(newN);

    // Clear all cells
    for (int i = 0; i < newN; i++) {
        for (int j = 0; j < newN; j++) {
            QTableWidgetItem* item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);

            if (i == j) {
                item->setText("0");
                item->setBackground(Qt::lightGray);
            }
            else {
                item->setText("INF");
                item->setBackground(QColor(255, 230, 230));
            }

            matrixTable->setItem(i, j, item);
        }
    }

    // Restore saved values
    for (const auto& saved : savedValues) {
        int i = saved.first.x();
        int j = saved.first.y();
        if (i < newN && j < newN) {
            QTableWidgetItem* item = matrixTable->item(i, j);
            if (item) {
                item->setText(saved.second);
                if (saved.second == "INF") {
                    item->setBackground(QColor(255, 230, 230));
                }
                else if (saved.second != "0") {
                    item->setBackground(Qt::white);
                }
            }
        }
    }

    // Update headers
    for (int i = 0; i < newN; i++) {
        QTableWidgetItem* header = new QTableWidgetItem(QString::number(i));
        header->setTextAlignment(Qt::AlignCenter);
        matrixTable->setHorizontalHeaderItem(i, header);
        matrixTable->setVerticalHeaderItem(i, new QTableWidgetItem(QString::number(i)));
    }

    // Update source/target if out of range
    if (source >= newN) {
        source = 0;
        sourceEdit->setText("0");
    }
    if (targetNode >= newN) {
        targetNode = newN - 1;
        targetEdit->setText(QString::number(targetNode));
    }

    n = newN;

    log(QString("✓ Graph resized to %1 nodes").arg(n));
    log("✓ Diagonal set to 0, other cells set to INF");
    log("✓ Edit cells directly or use Random Graph button");

    // Redraw
    readGraphFromTable();
    drawGraph();
}

// ================= Draw Graph =================
void FINALdataStructurePROJECT::drawGraph(int current)
{
    if (!scene) return;

    scene->clear();
    positions.clear();

    if (n <= 0) {
        scene->addText("No graph to display");
        return;
    }

    // Calculate positions in circle
    QRectF viewRect = view->viewport()->rect();
    QPointF center(viewRect.width() / 2.0, viewRect.height() / 2.0);
    double radius = std::min(viewRect.width(), viewRect.height()) * 0.35;

    for (int i = 0; i < n; i++) {
        double angle = 2.0 * M_PI * i / n;
        double x = center.x() + radius * std::cos(angle);
        double y = center.y() + radius * std::sin(angle);
        positions.append(QPointF(x, y));
    }

    // Draw edges with weights
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {  // Draw each edge once
            if (graph[i][j] != INF) {
                QLineF line(positions[i], positions[j]);
                line.setLength(line.length() - 25);  // Avoid node overlap

                QGraphicsLineItem* edge = scene->addLine(line);
                QPen edgePen(Qt::gray, 1.5);
                if (graph[i][j] < 5) edgePen.setColor(Qt::darkGreen);
                else if (graph[i][j] < 10) edgePen.setColor(Qt::darkBlue);
                else edgePen.setColor(Qt::darkRed);
                edge->setPen(edgePen);

                // Add weight
                QGraphicsTextItem* weight = scene->addText(QString::number(graph[i][j]));
                weight->setDefaultTextColor(edgePen.color());
                weight->setFont(QFont("Arial", 8, QFont::Bold));
                QPointF mid = line.pointAt(0.5);
                QRectF weightRect = weight->boundingRect();
                weight->setPos(mid.x() - weightRect.width() / 2,
                    mid.y() - weightRect.height() / 2);
            }
        }
    }

    // Draw nodes
    for (int i = 0; i < n; i++) {
        QColor nodeColor = QColor(200, 230, 255);  // Light blue default
        QColor textColor = Qt::black;

        if (i == current) {
            nodeColor = Qt::yellow;
            textColor = Qt::black;
        }
        else if (i == source) {
            nodeColor = QColor(0, 100, 255);  // Blue
            textColor = Qt::white;
        }
        else if (i == targetNode) {
            nodeColor = QColor(255, 100, 100);  // Red
            textColor = Qt::white;
        }
        else if (i < visited.size() && visited[i]) {
            nodeColor = QColor(100, 255, 100);
            textColor = Qt::black;
        }

        // Draw node
        QGraphicsEllipseItem* node = scene->addEllipse(
            positions[i].x() - 22, positions[i].y() - 22,
            44, 44, QPen(Qt::black, 2), QBrush(nodeColor));
        node->setZValue(2);

        // Node number
        QGraphicsTextItem* nodeText = scene->addText(QString::number(i));
        nodeText->setDefaultTextColor(textColor);
        nodeText->setFont(QFont("Arial", 10, QFont::Bold));
        QRectF textRect = nodeText->boundingRect();
        nodeText->setPos(positions[i].x() - textRect.width() / 2,
            positions[i].y() - textRect.height() / 2);
        nodeText->setZValue(3);

        // Distance label
        QString distText = (i < dist.size() && dist[i] != INF)
            ? QString::number(dist[i])
            : "∞";

        QGraphicsTextItem* distLabel = scene->addText("d=" + distText);
        distLabel->setDefaultTextColor(Qt::darkRed);
        distLabel->setFont(QFont("Arial", 8));
        QRectF distRect = distLabel->boundingRect();
        distLabel->setPos(positions[i].x() - distRect.width() / 2,
            positions[i].y() + 28);
        distLabel->setZValue(3);
    }
}

// ================= Other Methods =================
void FINALdataStructurePROJECT::animateStep()
{
    if (visitOrder.isEmpty()) {
        timer->stop();
        drawGraph();
        log("⚠ No nodes were visited (empty visit order)");
        return;
    }

    if (animIndex >= visitOrder.size()) {
        timer->stop();
        drawGraph();
        if (dist[targetNode] != INF) {
            drawPathTo(targetNode);
            log("✓ Animation complete - Shortest path highlighted!");
        }
        else {
            log("✓ Animation complete - Target unreachable");
        }
        return;
    }

    int current = visitOrder[animIndex];
    drawGraph(current);
    animIndex++;
}


void FINALdataStructurePROJECT::drawPathTo(int target)
{
    if (parent[target] == -1) return;

    QPen pathPen(QColor(255, 50, 50), 4);
    pathPen.setStyle(Qt::DashLine);

    int v = target;
    while (parent[v] != -1) {
        int u = parent[v];
        scene->addLine(positions[u].x(), positions[u].y(),
            positions[v].x(), positions[v].y(), pathPen);
        v = u;
    }
}

void FINALdataStructurePROJECT::addStepToTable(int currentNode)
{
    int row = stepsTable->rowCount();
    stepsTable->insertRow(row);

    stepsTable->setItem(row, 0,
        new QTableWidgetItem(QString::number(iterationCount++)));
    stepsTable->setItem(row, 1,
        new QTableWidgetItem(QString::number(currentNode)));

    for (int i = 0; i < n; i++) {
        QString dText = (dist[i] == INF) ? "INF" : QString::number(dist[i]);
        QTableWidgetItem* item = new QTableWidgetItem(dText);
        if (i == currentNode) {
            item->setBackground(Qt::yellow);
        }
        else if (dist[i] != INF && dist[i] < 1000) {
            item->setBackground(QColor(200, 255, 200));
        }
        stepsTable->setItem(row, i + 2, item);
    }
}

void FINALdataStructurePROJECT::log(const QString& text)
{
    if (logBox) {
        logBox->append(text);
        QTextCursor cursor = logBox->textCursor();
        cursor.movePosition(QTextCursor::End);
        logBox->setTextCursor(cursor);
    }
}

void FINALdataStructurePROJECT::exportStepsToFile()
{
    log("Export feature would save steps to file");
    // Implementation for file export
}