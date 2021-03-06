// ***********************************************************************************************
// *                                                                                             *
// * setFilename.cpp - sets the output filename                                                  *
// *                                                                                             *
// * Dr. Rainer Sieger - 2009-10-14                                                              *
// *                                                                                             *
// ***********************************************************************************************

#include "Application.h"

/*! @brief Setzt den Output Dateinamen. */

int MainWindow::setFilename( const int i_Mode, const bool b_CuratorMode, const int i_NumOfFiles, const QString &s_FilenameIn, QString &s_FilenameOut )
{
    int i_DialogResult = QDialog::Rejected;

// ***********************************************************************************************

    QFileInfo fileIn( s_FilenameIn );
    QDir dirIn( fileIn.absolutePath() );

    if ( dirIn.exists() == true )
        s_FilenameOut = QDir::toNativeSeparators( fileIn.absolutePath() );
    else
        s_FilenameOut = getDocumentDir();

    if ( i_NumOfFiles == 1 )
        s_FilenameOut.append( QDir::toNativeSeparators( "/" ) + fileIn.baseName() );

// ***********************************************************************************************

    switch ( i_Mode )
    {
        case _FORMAT_KMLFILE:
            s_FilenameOut.append( ".kml" );
            break;

        case _FORMAT_ODV:
            s_FilenameOut.replace( "_odv", "" );
            s_FilenameOut.append( "_odv.txt" );
            break;

        case _FORMAT_SHAPE_METADATA:
        case _FORMAT_SHAPE_DATA:
            s_FilenameOut.append( ".shp" );
            break;

        case _FORMAT_FORMATEDTEXTFILE:
            s_FilenameOut.replace( "_formated", "" );
            s_FilenameOut.append( "_formated.txt" );
            break;

        case _FORMAT_UNFORMATEDTEXTFILE:
            s_FilenameOut.replace( "_out", "" );
            s_FilenameOut.append( "_out.txt" );
            break;

        default:
            s_FilenameOut.replace( "_out", "" );
            s_FilenameOut.append( "_out.txt" );
            break;
    }

// ***********************************************************************************************

    switch ( i_Mode )
    {
        case _FORMAT_KMLFILE:
            i_DialogResult = doGoogleEarthOptionsDialog();
            break;

        case _FORMAT_ODV:
            i_DialogResult = doOceanDataViewOptionsDialog();
            break;

        case _FORMAT_SHAPE_METADATA:
            ; // no dialog
            break;

        case _FORMAT_SHAPE_DATA:
            i_DialogResult = doShapeFileOptionsDialog();
            break;

        case _FORMAT_UNFORMATEDTEXTFILE:
            if ( b_CuratorMode == false )
                i_DialogResult = doUnformatedTextOptionsDialog();
            else
                i_DialogResult = QDialog::Accepted;
            break;

        case _FORMAT_FORMATEDTEXTFILE:
            i_DialogResult = doFormatedTextOptionsDialog();
            break;

        default:
            i_DialogResult = doUnformatedTextOptionsDialog();
            break;
    }

    return( i_DialogResult );
}
