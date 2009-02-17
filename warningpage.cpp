#include "warningpage.h"

WarningPage::WarningPage(QWidget *parent) : QWizardPage(parent)
{
  QLabel *text = new QLabel("You have selected to start from a clean KDE and to not backup data. You will most likely use data here. Are you sure?");
  QHBoxLayout *lay = new QHBoxLayout();
  lay->addWidget(text);
  setLayout(lay);
}