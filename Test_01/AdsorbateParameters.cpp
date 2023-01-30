#include "Include.h"

AdsorbateParameters::AdsorbateParameters(QDialog* parent) :QDialog(parent)
{
	ui.setupUi(this);

	ui.textEdit->append(("77.350"));
	ui.textEdit_2->append(("28.0134"));
	ui.textEdit_3->append(("16.200"));
	ui.textEdit_4->append(("0.806"));
	ui.textEdit_5->append(("6.5800e-05"));
}

AdsorbateParameters::~AdsorbateParameters() {

}