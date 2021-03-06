/* 2016-08-17                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-08-17

int MainWindow::extractExif( const QString &s_ExifTool, const QStringList &sl_FilenameList, const QString &s_FilenameOut, const int i_DateTimeFormat, const int i_UtcOffset )
{
    int         i                = 0;
    int         n                = 0;
    int         err              = 0;
    int         stopProgress     = 0;

    QString     s_DateTimeFormat = "";

    QString     s_arg            = "";

    QStringList sl_Input;

    QProcess    process;

// **********************************************************************************************

    switch( i_DateTimeFormat )
    {
    case _BUILDDATE:
        s_DateTimeFormat = "yyyy-MM-dd";
        break;

    case _BUILDDATETIME:
        s_DateTimeFormat = "yyyy-MM-dd hh:mm";
        break;

    case _BUILDISODATETIME:
        s_DateTimeFormat = "yyyy-MM-ddThh:mm:ss";
        break;

    default:
        s_DateTimeFormat = "yyyy-MM-ddThh:mm";
        break;
    }

// **********************************************************************************************

    QFileInfo fiOut( s_FilenameOut );
    QFileInfo fiKML( fiOut.absolutePath() + "/" + fiOut.baseName() + ".kml" );

// **********************************************************************************************

    if ( fiOut.exists() == true )
        QFile::remove( fiOut.absoluteFilePath() );

    if ( fiKML.exists() == true )
        QFile::remove( fiKML.absoluteFilePath() );

// **********************************************************************************************

    initFileProgress( sl_FilenameList.count(), sl_FilenameList.at( 0 ), tr( "Extracting exif record ..." ) );

// **********************************************************************************************
    // ExifTool -n -T -gpsdatetime -gpslatitude -gpslongitude -copyright image.jpg >> out.txt

        while ( ( i < sl_FilenameList.count() ) && ( stopProgress != _APPBREAK_ ) )
        {
            QFileInfo fiIn( sl_FilenameList.at( i ) );

            setStatusBar( tr( "Run exiftool on " ) + QDir::toNativeSeparators( fiIn.fileName() ) + tr( " ..." ) );

            if ( ( fiIn.suffix().toLower() == "jpg" ) || ( fiIn.suffix().toLower() == "jpeg" ) || ( fiIn.suffix().toLower() == "tif" ) || ( fiIn.suffix().toLower() == "tiff" ) || ( fiIn.suffix().toLower() == "png" ) )
            {
                s_arg = s_ExifTool;

                s_arg.append( " -n -T -GPSDateTime -GPSLatitude -GPSLongitude -GPSAltitude -Copyright" );
                s_arg.append( " -w txt" );
                s_arg.append( " " ).append( "\"" + QDir::toNativeSeparators( fiIn.absoluteFilePath() ) + "\"" );

                process.start( s_arg );
                process.waitForFinished( -1 );

                wait( 500 );
            }

            stopProgress = incFileProgress( sl_FilenameList.count(), ++i );
        }

        resetFileProgress( sl_FilenameList.count() );

// **********************************************************************************************
// create output file

        if ( stopProgress != _APPBREAK_ )
        {
            QFile fout( fiOut.absoluteFilePath() );

            if ( fout.open( QIODevice::WriteOnly | QIODevice::Text) == false )
                return( -20 );

            QTextStream tout( &fout );

            tout << "File name" << "\t" << "Date/Time (UTC)" << "\t";

            if ( i_UtcOffset != 0 )
               tout << "Date/Time (local)" << "\t";

            tout << "Latitude" << "\t" << "Longitude" << "\t" << "Altitude [m]" << "\t" << "Copyright" << endl;

            for ( int i=0; i<sl_FilenameList.count(); i++ )
            {
                QFileInfo fiIn( sl_FilenameList.at( i ) );

                if ( ( fiIn.suffix().toLower() == "jpg" ) || ( fiIn.suffix().toLower() == "jpeg" ) || ( fiIn.suffix().toLower() == "tif" ) || ( fiIn.suffix().toLower() == "tiff" ) || ( fiIn.suffix().toLower() == "png" ) )
                {
                    if ( ( n = readFile( fiIn.absolutePath() + "/" + fiIn.completeBaseName() + ".txt", sl_Input, _SYSTEM_ ) ) < 1 )
                    {
                        tout << fiIn.fileName() << "\t";
                        tout << "no exif data found" << endl;
                    }
                    else
                    {
                        QString s_DateTime   = sl_Input.at( 0 ).section( "\t", 0, 0 ).section( " ", 0, 0 ).replace( ":", "-" ) + "T" + sl_Input.at( 0 ).section( "\t", 0, 0 ).section( " ", 1, 1 ).section( "Z", 0, 0 ).section( ".", 0, 0 );
                        double  d_Latitude   = sl_Input.at( 0 ).section( "\t", 1, 1 ).toDouble();
                        double  d_Longitude  = sl_Input.at( 0 ).section( "\t", 2, 2 ).toDouble();
                        double  d_Altitude   = sl_Input.at( 0 ).section( "\t", 3, 3 ).toDouble();
                        QString s_Copyright  = sl_Input.at( 0 ).section( "\t", 4, 4 );

                        QDateTime dtUtc      = QDateTime::fromString( s_DateTime, "yyyy-MM-ddThh:mm:ss" );
                        QDateTime dtLocal    = QDateTime::fromString( s_DateTime, "yyyy-MM-ddThh:mm:ss" ).addSecs( i_UtcOffset );

                        tout << sl_FilenameList.at( i ) << "\t";
                        tout << dtUtc.toString( s_DateTimeFormat ) << "\t";

                        if ( i_UtcOffset != 0 )
                            tout << dtLocal.toString( s_DateTimeFormat ) << "\t";

                        tout << QString( "%1" ).arg( d_Latitude, 0, 'f', 6 ) << "\t";
                        tout << QString( "%1" ).arg( d_Longitude, 0, 'f', 6 ) << "\t";
                        tout << QString( "%1" ).arg( d_Altitude, 0, 'f', 1 ) << "\t";

                        if ( s_Copyright != "-" )
                            tout << s_Copyright;

                        tout << endl;
                    }

                    removeFile( fiIn.absolutePath() + "/" + fiIn.completeBaseName() + ".txt" );
                }
            }

            fout.close();
        }
    else
    {
        err = _APPBREAK_;
    }

    return( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-08-17

void MainWindow::doExtractExif()
{
    int		  err            = 0;
    int       stopProgress   = 0;

    QString   s_ExifTool     = "";

// **********************************************************************************************

    s_ExifTool = findExifTool();

    if ( s_ExifTool != "ExifTool not found" )
    {
        existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList );

        if ( ( gsl_FilenameList.count() > 0 ) && ( doExifToolOptionsDialog() == QDialog::Accepted ) )
            err = extractExif( s_ExifTool, gsl_FilenameList, gs_FilenameExifOut, gi_DateTimeFormat, gi_UtcOffset );
        else
            err = _CHOOSEABORTED_;
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    if ( ( err == _NOERROR_ ) && ( gb_CreateKmlFile == true ) )
    {
        gsl_FilenameList.clear();
        gsl_FilenameList.append( gs_FilenameExifOut );
        doCreateGoogleEarthImportFile();
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Extracting exif record was canceled" ), true );

    onError( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// check the availability of ExifTool

QString MainWindow::findExifTool()
{
#if defined(Q_OS_LINUX)
    QFileInfo fi_ExifTool( "/usr/bin/exiftool" );

    if ( fi_ExifTool.exists() == false )
    {
        QMessageBox::information( this, getApplicationName( true ), tr( "You have to install the\nprogram ExifTool (http://www.sno.phy.queensu.ca/~phil/exiftool/)\nbefore using this function." ) );

        return( "ExifTool not found" );
    }
#endif

#if defined(Q_OS_MAC)
    QFileInfo fi_ExifTool( "/usr/local/bin/exiftool" );

    if ( fi_ExifTool.exists() == false )
    {
        QMessageBox::information( this, getApplicationName( true ), tr( "You have to install the\nprogram ExifTool (http://www.sno.phy.queensu.ca/~phil/exiftool/)\nbefore using this function." ) );

        return( "ExifTool not found" );
    }
#endif

#if defined(Q_OS_WIN)
    QFileInfo fi_ExifTool( QCoreApplication::applicationDirPath() + "/" + "exiftool.exe" );

    if ( fi_ExifTool.exists() == false )
    {
        QMessageBox::information( this, getApplicationName( true ), tr( "You have to install the\nprogram ExifTool (http://www.sno.phy.queensu.ca/~phil/exiftool/)\nbefore using this function." ) );

        return( "ExifTool not found" );
    }
#endif

    return( tr( "\"" ) + fi_ExifTool.absoluteFilePath() + tr( "\"" ) );
}
