#include "musicfileinformationwidget.h"
#include "ui_musicfileinformationwidget.h"
#include "musicuiobject.h"
#include "musicurlutils.h"
#include "musicnumberutils.h"
#include "musicsongtag.h"
#include "musictoastlabel.h"
#include "musicfileutils.h"
#include "musicmessagebox.h"

#define ADVANCE_OFFSET  150

MusicFileInformationWidget::MusicFileInformationWidget(QWidget *parent)
    : MusicAbstractMoveDialog(parent),
      m_ui(new Ui::MusicFileInformationWidget)
{
    m_ui->setupUi(this);
    setFixedSize(size());
    
    m_ui->topTitleCloseButton->setIcon(QIcon(":/functions/btn_close_hover"));
    m_ui->topTitleCloseButton->setStyleSheet(MusicUIObject::MQSSToolButtonStyle04);
    m_ui->topTitleCloseButton->setCursor(QCursor(Qt::PointingHandCursor));
    m_ui->topTitleCloseButton->setToolTip(tr("Close"));
    connect(m_ui->topTitleCloseButton, SIGNAL(clicked()), SLOT(close()));

    setStyleSheet(MusicUIObject::MQSSLineEditStyle01);
    setEditLineEnabled(false);

    m_advanceOn = false;
    m_deleteOn = false;
    advanceClicked();

    QPixmap pix(":/image/lb_defaultArt");
    m_ui->pixmapLabel->setPixmap(pix.scaled(m_ui->pixmapLabel->size()));

    m_ui->editButton->setStyleSheet(MusicUIObject::MQSSPushButtonStyle04);
    m_ui->deletePixButton->setStyleSheet(MusicUIObject::MQSSPushButtonStyle04);
    m_ui->savePixButton->setStyleSheet(MusicUIObject::MQSSPushButtonStyle04);
    m_ui->saveButton->setStyleSheet(MusicUIObject::MQSSPushButtonStyle04);
    m_ui->viewButton->setStyleSheet(MusicUIObject::MQSSPushButtonStyle04);
    m_ui->openPixButton->setStyleSheet(MusicUIObject::MQSSPushButtonStyle04);

#ifdef Q_OS_UNIX
    m_ui->editButton->setFocusPolicy(Qt::NoFocus);
    m_ui->deletePixButton->setFocusPolicy(Qt::NoFocus);
    m_ui->savePixButton->setFocusPolicy(Qt::NoFocus);
    m_ui->saveButton->setFocusPolicy(Qt::NoFocus);
    m_ui->openPixButton->setFocusPolicy(Qt::NoFocus);
    m_ui->viewButton->setFocusPolicy(Qt::NoFocus);
#endif

    connect(m_ui->editButton, SIGNAL(clicked()), SLOT(editTag()));
    connect(m_ui->deletePixButton, SIGNAL(clicked()), SLOT(deleteAlbumPicture()));
    connect(m_ui->savePixButton, SIGNAL(clicked()), SLOT(saveAlbumPicture()));
    connect(m_ui->saveButton, SIGNAL(clicked()), SLOT(saveTag()));
    connect(m_ui->viewButton, SIGNAL(clicked()), SLOT(openFileDir()));
    connect(m_ui->advanceLabel, SIGNAL(clicked()), SLOT(advanceClicked()));
    connect(m_ui->openPixButton, SIGNAL(clicked()), SLOT(openImageFileDir()));
}

MusicFileInformationWidget::~MusicFileInformationWidget()
{
    delete m_ui;
}

void MusicFileInformationWidget::openFileDir()
{
    if(!MusicUtils::Url::openUrl(QFileInfo(m_path).absoluteFilePath()))
    {
        MusicToastLabel::popup(tr("The origin one does not exist!"));
    }
}

void MusicFileInformationWidget::openImageFileDir()
{
    m_imagePath = MusicUtils::File::getOpenFileDialog(this);
    if(m_imagePath.isEmpty())
    {
        return;
    }

    QPixmap pix;
    pix.load(m_imagePath);
    m_ui->pixmapSizeLabel->setText(QString("%1x%2").arg(pix.width()).arg(pix.height()));
    m_ui->pixmapLabel->setPixmap(pix.scaled(m_ui->pixmapLabel->size()));
}

void MusicFileInformationWidget::advanceClicked()
{
    if(m_advanceOn)
    {
        setFixedHeight(420 + ADVANCE_OFFSET);
        m_ui->background->setFixedHeight(412 + ADVANCE_OFFSET);
        m_ui->backgroundMask->setFixedHeight(387 + ADVANCE_OFFSET);
        m_ui->advanceLabel->move(29, 380 + ADVANCE_OFFSET);
        m_ui->editButton->move(310, 345 + ADVANCE_OFFSET);
        m_ui->saveButton->move(390, 345 + ADVANCE_OFFSET);
        m_ui->pixmapLabel->setVisible(true);
        m_ui->label_17->setVisible(true);
        m_ui->decoderLabel->setVisible(true);

        QPixmap pix;
        MusicSongTag tag;
        if(tag.read(m_path))
        {
            pix = tag.getCover();
        }

        QString text = QString("%1x%2").arg(pix.width()).arg(pix.height());
        if(pix.isNull())
        {
            text = STRING_NULL;
            pix.load(":/image/lb_defaultArt");
        }
        m_ui->pixmapSizeLabel->setText(text);
        m_ui->pixmapLabel->setPixmap(pix.scaled(m_ui->pixmapLabel->size()));
    }
    else
    {
        setFixedHeight(420);
        m_ui->background->setFixedHeight(412);
        m_ui->backgroundMask->setFixedHeight(387);
        m_ui->advanceLabel->move(29, 380);
        m_ui->editButton->move(310, 345);
        m_ui->saveButton->move(390, 345);
        m_ui->pixmapLabel->setVisible(false);
        m_ui->label_17->setVisible(false);
        m_ui->decoderLabel->setVisible(false);
    }

    m_advanceOn = !m_advanceOn;
    setBackgroundPixmap(m_ui->background, size());
}

void MusicFileInformationWidget::deleteAlbumPicture()
{
    QPixmap pix(":/image/lb_defaultArt");
    m_ui->pixmapSizeLabel->setText(STRING_NULL);
    m_ui->pixmapLabel->setPixmap(pix.scaled(m_ui->pixmapLabel->size()));
    m_deleteOn = true;
}

void MusicFileInformationWidget::saveAlbumPicture()
{
    QPixmap pix;
    MusicSongTag tag;
    if(tag.read(m_path))
    {
        pix = tag.getCover();
    }

    if(!pix.isNull())
    {
        const QString &filename = MusicUtils::File::getSaveFileDialog(this);
        if(!filename.isEmpty())
        {
            pix.save(filename);
        }
    }
}

void MusicFileInformationWidget::editTag()
{
    setEditLineEnabled(!m_ui->fileAlbumEdit->isEnabled());
}

void MusicFileInformationWidget::saveTag()
{
    MusicMessageBox message;
    message.setText(tr("Are you sure to save?"));
    if(!message.exec())
    {
       return;
    }

    MusicSongTag tag;
    if(!tag.read(m_path))
    {
        return;
    }

    QString value = m_ui->fileAlbumEdit->text().trimmed();
    if(value != STRING_NULL)
    {
        tag.setAlbum(value);
    }

    value = m_ui->fileArtistEdit->text().trimmed();
    if(value != STRING_NULL)
    {
        tag.setArtist(value);
    }

    value = m_ui->fileGenreEdit->text().trimmed();
    if(value != STRING_NULL)
    {
        tag.setGenre(value);
    }

    value = m_ui->fileTitleEdit->text().trimmed();
    if(value != STRING_NULL)
    {
        tag.setTitle(value);
    }

    value = m_ui->fileYearEdit->text().trimmed();
    if(value != STRING_NULL)
    {
        tag.setYear(value);
    }

    if(m_deleteOn)
    {
        tag.setCover(QPixmap());
    }
    else if(!m_imagePath.isEmpty())
    {
        tag.setCover(QPixmap(m_imagePath));
    }

    tag.save();

    MusicToastLabel::popup(tr("Save Successfully!"));
}

void MusicFileInformationWidget::setFileInformation(const QString &name)
{
    if(name.contains(CACHE_DIR_FULL))
    {
        m_ui->viewButton->setEnabled(false);
    }
    //cache song should not allow open url

    MusicSongTag tag;
    const bool state = tag.read(m_path = name);
    const QFileInfo fin(name);

    QString check;
    m_ui->filePathEdit->setText((check = name).isEmpty() ? STRING_NULL : check);
    m_ui->fileFormatEdit->setText((check = fin.suffix()).isEmpty() ? STRING_NULL : check);
    m_ui->fileSizeEdit->setText((check = MusicUtils::Number::size2Label(fin.size())).isEmpty() ? STRING_NULL : check);

    m_ui->fileAlbumEdit->setText(state ? ((check = tag.getAlbum()).isEmpty() ? STRING_NULL : check) : STRING_NULL);
    m_ui->fileArtistEdit->setText(state ? ((check = tag.getArtist()).isEmpty() ? STRING_NULL : check) : STRING_NULL);
    m_ui->fileGenreEdit->setText(state ? ((check = tag.getGenre()).isEmpty() ? STRING_NULL : check) : STRING_NULL);
    m_ui->fileTitleEdit->setText(state ? ((check = tag.getTitle()).isEmpty() ? STRING_NULL : check) : STRING_NULL);
    m_ui->fileYearEdit->setText(state ? ((check = tag.getYear()).isEmpty() ? STRING_NULL : check) : STRING_NULL);
    m_ui->fileTimeEdit->setText(state ? ((check = tag.getLengthString()).isEmpty() ? STRING_NULL : check) : STRING_NULL);

    m_ui->BitrateEdit->setText(state ? ((check = (tag.getBitrate())).isEmpty() ? STRING_NULL : check) : STRING_NULL);
    m_ui->ChannelEdit->setText(state ? ((check = tag.getChannel()).isEmpty() ? STRING_NULL : check) : STRING_NULL);
    m_ui->SamplingRateEdit->setText(state ? ((check = tag.getSampleRate()).isEmpty() ? STRING_NULL : check) : STRING_NULL);
    m_ui->TrackNumEdit->setText(state ? ((check = tag.getTrackNum()).isEmpty() ? STRING_NULL : check) : STRING_NULL);
    m_ui->decoderLabel->setText(state ? ((check = tag.getDecoder()).isEmpty() ? STRING_NULL : check.toUpper()) : STRING_NULL);
    m_ui->qualityEdit->setText(MusicUtils::Number::transfromBitrateToQuality(MusicUtils::Number::transfromBitrateToLevel(m_ui->BitrateEdit->text())));
}

void MusicFileInformationWidget::setEditLineEnabled(bool enable)
{
    m_ui->fileAlbumEdit->setEnabled(enable);
    m_ui->fileArtistEdit->setEnabled(enable);
    m_ui->fileGenreEdit->setEnabled(enable);
    m_ui->fileTitleEdit->setEnabled(enable);
    m_ui->fileYearEdit->setEnabled(enable);

    m_ui->saveButton->setEnabled(enable);
    m_ui->deletePixButton->setEnabled(enable);

    m_ui->openPixButton->setEnabled(enable);
}

int MusicFileInformationWidget::exec()
{
    setBackgroundPixmap(m_ui->background, size());
    return MusicAbstractMoveDialog::exec();
}
