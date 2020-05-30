#-------------------------------------------------------------------------------
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad
import binascii
import os
from base64 import b64encode
from Crypto.Util.Padding import pad
from sys import argv
#-------------------------------------------------------------------------------
key = bytes([ 0x83, 0xF7, 0x79, 0x7F, 0x52, 0x1E, 0x37, 0xA2, 0x6B, 0xAF, 0xBB, 0xD0, 0x41, 0x77, 0x9A, 0xB5 ]);
iv  = bytes([ 0x49, 0x60, 0x7B, 0x42, 0x55, 0xE6, 0xE9, 0x4B, 0x3C, 0xC7, 0x76, 0xFB, 0x06, 0x67, 0xA9, 0xF2 ]);
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
def openFile( name ):
    return open( name, encoding="utf-8" ).read();
#-------------------------------------------------------------------------------
def encyptLine( line ):
    out = line + "\n";
    if ( len( line ) > 0 ):
        length = int( line[1:3], 16 );
        data   = [];
        if ( int( line[7:9], 16 ) == 0x00 ):
            count = 9;
            for i in range( 0, length ):
                data.append( int( line[ count : ( count + 2 ) ], 16 ) );
                count += 2;
            for i in range( len( data ), 16 ):
                data.append( 0 );
            enc  = encrypt( data );
            crc  = 0;
            data = "";
            for i in range( 0, length ):
                data += format( enc[i], '02X' );
                crc  += enc[i];
            crc = format( ( 1 + ( ~crc & 0xFF ) ) & 0xFF , '02X' );
            out =  line[0:8] + data + "\n";
    return out;
#-------------------------------------------------------------------------------
def encrypt ( line ):
    chunk = bytes( line );
    return cipher.encrypt( chunk );
#-------------------------------------------------------------------------------
if __name__ == "__main__":
    if ( len( argv ) > 1 ):
        cipher    = AES.new( key, AES.MODE_CBC, iv=iv );
        strInd    = 0;
        lineCount = 0;
        outFile   = "";
        file   = openFile( argv[1] );
        while( True ):
            strInd     = file.find( ":", strInd );
            endInd     = file.find( "\n", strInd );
            line       = file[strInd:endInd];
            strInd    += 1;
            lineCount += 1;
            newLine    = encyptLine( line );
            outFile   += newLine;
            if ( strInd == 0 ):
                break;
        output = open( "encrypt.hex", "w+" );
        output.write( outFile );
        print( "DONE!" )
    else:
        print( "No file name. Print file name as parameter" );
#-------------------------------------------------------------------------------
