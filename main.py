import sys
from PySide6.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget, QLabel
from PySide6.QtCharts import QChart, QChartView, QLineSeries, QValueAxis
from PySide6.QtCore import Qt
from PySide6.QtGui import QPainter


app = QApplication(sys.argv)

# Create the line series
series = QLineSeries()
series.append(0, 6)
series.append(2, 4)
series.append(3, 8)
series.append(7, 4)
series.append(10, 5)
series.append(11, 1)
series.append(13, 3)
series.append(17, 6)
series.append(18, 3)
series.append(20, 2)

# Create the chart and add the series to it
chart = QChart()
chart.legend().hide()
chart.addSeries(series)

# Create and configure the axes
axisX = QValueAxis()
axisX.setTitleText('X Axis Label')
chart.addAxis(axisX, Qt.AlignBottom)
series.attachAxis(axisX)

axisY = QValueAxis()
axisY.setTitleText('Y Axis Label')
chart.addAxis(axisY, Qt.AlignLeft)
series.attachAxis(axisY)

chart.setTitle('Simple Line Chart Example')

# Create the ChartView
chart_view = QChartView(chart)
chart_view.setRenderHint(QPainter.Antialiasing)

# Create a QLabel
label = QLabel('This is a QLabel with some text')

# Set up layout and central widget
main_widget = QWidget()
layout = QVBoxLayout(main_widget)
layout.addWidget(label)  # Add label to layout
layout.addWidget(chart_view)  # Add chart view to layout

# Set up main window
window = QMainWindow()
window.setCentralWidget(main_widget)
window.resize(400, 300)
window.show()

sys.exit(app.exec())
