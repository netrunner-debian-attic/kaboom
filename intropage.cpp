#include "intropage.h"

IntroPage::IntroPage(QWidget *parent) : QWizardPage(parent)
{
  setTitle("Kaboom - MigrationTool");
  QLabel *text = new QLabel(tr("This wizard will guide you thru the migration of KDE 3 and KDE 4 settings as required"));
  QVBoxLayout *lay = new QVBoxLayout();
  lay->addWidget(text);
  setLayout(lay);
}