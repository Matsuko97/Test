#include "Include.h"

PlotWindow::PlotWindow(QWidget* parent):QMainWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_QuitOnClose, false);
}

PlotWindow::~PlotWindow() {

}