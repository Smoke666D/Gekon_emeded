#-------------------------------------------------------------------------------
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad
import binascii
import os
from base64 import b64encode
from Crypto.Util.Padding import pad
from sys import argv
import math
#-------------------------------------------------------------------------------
key = bytes([ 0x83, 0xF7, 0x79, 0x7F, 0x52, 0x1E, 0x37, 0xA2, 0x6B, 0xAF, 0xBB, 0xD0, 0x41, 0x77, 0x9A, 0xB5 ]);
iv  = bytes([ 0x49, 0x60, 0x7B, 0x42, 0x55, 0xE6, 0xE9, 0x4B, 0x3C, 0xC7, 0x76, 0xFB, 0x06, 0x67, 0xA9, 0xF2 ]);
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
class Line:
    def __init__( self, str, shift ):
        count        = 9;
        calcCrc      = 0;
        self.raw     = str;
        self.valid   = 1;
        self.len     = int( str[1:3], 16 );
        self.adr     = shift + int( str[3:7], 16 );
        self.type    = int( str[7:9], 16 );
        self.data    = [];
        for i in range( 0, self.len ):
            byte = int( str[ count : ( count + 2 ) ], 16 );
            self.data.append( byte );
            calcCrc += byte;
            count   += 2;
        calcCrc += self.len + ( self.adr & 0xFF ) + ( ( self.adr & 0xFF00 ) >> 8 ) + self.type;
        calcCrc = ( ( ~( calcCrc & 0xFF) & 0xFF ) + 0x01 ) & 0xFF;
        self.crc = int( str[ count : ( count + 2 ) ], 16 );
        if ( calcCrc != self.crc ):
            self.valid = 0;
        return;
class HexFile:
    start = 0x08000000;
    end   = 0x08000000;
    size  = 0;
    valid = 0;
    data  = [];
    def __init__( self, str ):
        firstData = 0;
        prevAdr   = 0;
        prevLen   = 0;
        adrShift  = 0;
        buffer    = file;
        index     = 0;
        endLine   = 0;
        line      = "";
        while ( index > -1 ):
            index   = buffer.find( ':' );
            endLine = buffer.find( '\n' );
            if ( ( index > -1 ) and ( endLine > -1 ) ):
                line    = Line( buffer[ index : endLine ], adrShift );
                if ( line.valid > 0 ):
                    buffer  = buffer[ ( endLine + 1): len( buffer ) ];
                    if ( line.type == 0x04 ):
                        adrShift = 0;
                        for i in range( 0, line.len ):
                            adrShift |= line.data[i] << ( 8 * ( line.len - i - 1 ) )
                        adrShift = adrShift << 16;
                    if ( line.type == 0x01 ):
                        self.size  = len( self.data );
                        self.valid = 1;
                        index      = -1;
                    if ( line.type == 0x00 ):
                        if ( firstData == 0 ):
                            firstData   = 1;
                            self.start  = line.adr;
                            prevAdr     = line.adr;
                            prevLen     = line.len;
                            for i in range( 0, line.len ):
                                self.data.append( line.data[i] );
                        else:
                            shift = line.adr - prevAdr - prevLen;
                            for i in range( 0, shift ):
                                self.data.append( 0xFF );
                            for i in range( 0, line.len ):
                                self.data.append( line.data[i] );
                            prevAdr = line.adr;
                            prevLen = line.len;
                    self.end = line.adr + line.len;
                else:
                    console.log( line );
                    self.valid = 0;
                    index      = -1;
        mod = self.size % 16;
        if ( mod > 0 ):
            for i in range( 0 , ( 16 - mod ) ):
                self.size += 1;
                self.data.append( 0xFF );
        return;
    def encrypt( self ):
        count  = 0;
        out    = [];
        cipher = AES.new( key, AES.MODE_CBC, iv=iv );
        for i in range( 0, int( self.size/16 ) ):
            chunk  = [];
            for j in range( 0, 16 ):
                chunk.append( self.data[count] );
                count += 1;
            enc = cipher.encrypt( bytes( chunk ) );
            for j in range( 0, 16 ):
                out.append( int( format( enc[j], '02X' ), 16 ) );
        for i in range( 0, len( self.data ) ):
            self.data[i] = out[i];
        return;
    def save( self ):
        out       = "";
        sectors   = math.ceil( self.size / 0x10000 );
        shift     = self.start & 0x0000FFFF;
        dataCount = 0;
        for sector in range( 0, sectors ):
            if ( sector > 0 ):
                shift = 0;
            secShift = ( ( self.start + sector*0x10000 ) & 0xFFFF0000 ) >> 16;
            crc      = ( ~( ( 0x06 + ( ( secShift & 0xFF00 ) >> 8 ) + ( secShift & 0xFF ) ) & 0xFF ) + 0x01 ) & 0xFF;
            out     += ":02000004" + format( secShift, '04X' ) + format( crc, '02X' ) + "\n";
            for i in range( int( shift / 0x10 ), 0x1000 ):
                crc   = 0x10 + ( shift & 0xFF ) + ( ( shift & 0xFF00 ) >> 8 );
                line  = ":10" + format( shift, '04X') + "00";
                shift = ( shift + 0x10 ) & 0xFFFF;
                if ( dataCount < len( self.data ) ):
                    for j in range( 0, 0x10 ):
                        line += format( self.data[dataCount], '02X' );
                        crc  += self.data[dataCount];
                        dataCount += 1;
                    crc   = ( ( ( ~crc ) & 0xFF ) + 0x01 ) & 0xFF;
                    line += format( crc, '02X' ) + "\n";
                    out  += line;
        out += ":00000001FF\n";
        output = open( "encrypt.hex", "w+" );
        output.write( out );
        return;
#-------------------------------------------------------------------------------
def openFile( name ):
    return open( name, encoding="utf-8" ).read();
#-------------------------------------------------------------------------------
def encyptLine( line ):
    out = line;
    if ( len( line ) > 1 ):
        length = int( line[1:3], 16 );
        crc    = length;
        data   = [];
        adr    = int( line[3:7], 16 );
        crc   += int( line[3:5], 16 ) + int( line[5:7], 16 );
        type   = int( line[7:9], 16 );
        crc   += type;
        if ( type == 0x00 ):
            count = 9;
            for i in range( 0, length ):
                data.append( int( line[ count : ( count + 2 ) ], 16 ) );
                count += 2;
            for i in range( len( data ), 16 ):
                data.append( 0xFF );
            enc  = encrypt( data );
            data = "";
            for i in range( 0, length ):
                data += format( enc[i], '02X' );
                crc  += enc[i];
            crc = format( ( ( ~( crc & 0xFF) & 0xFF ) + 0x01 ) & 0xFF , '02X' );
            out =  line[0:9] + data + crc + "\n";
    return out;
#-------------------------------------------------------------------------------
def encrypt ( line ):
    chunk = bytes( line );
    return cipher.encrypt( chunk );
#-------------------------------------------------------------------------------
if __name__ == "__main__":
    if ( len( argv ) > 1 ):
        print("**************************************************************");
        cipher    = AES.new( key, AES.MODE_CBC, iv=iv );
        strInd    = 0;
        lineCount = 0;
        outFile   = "";
        print( "Reading HEX file..." );
        file   = openFile( argv[1] );
        hexFile = HexFile( file );
        print( "Encrypt HEX file..." );
        hexFile.encrypt();
        print( "Save HEX file..." );
        hexFile.save();

        print( "DONE!" )
    else:
        print( "No file name. Print file name as parameter" );
    print("**************************************************************");
#-------------------------------------------------------------------------------
