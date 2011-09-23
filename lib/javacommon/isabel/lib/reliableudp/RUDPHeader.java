/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package isabel.lib.reliableudp;

/**
 * This class represents the header of an RUDPPacket.
 *
 * /0----3-------7------------15/
 * /-VER-/S/R/A/D/--------------/
 * /-----------SEQ#-------------/
 * /-----------LENGTH-----------/
 * /----------CHECKSUM----------/
 *
 * @author Fernando Escribano
 */
class RUDPHeader {

    /**
     * Header Length
     */
    public static final int HEADER_LENGTH = 8;

    /**
     * Protocol version
     */
    public static final byte VERSION = 1;

    /**
     * byte[] to hold the header data
     */
    private byte[] header;

    // Bit of the first byte that hold the protocol flags
    private static final int SYN_OPT = 4;
    private static final int RST_OPT = 5;
    private static final int ACK_OPT = 6;
    private static final int DAT_OPT = 7;

    // Offset in bytes of the different fields of the header
    private static final int SEQ_OFF = 2;
    private static final int LEN_OFF = 4;
    private static final int CHK_OFF = 6;

    /**
     * Builds a new header with empty fields
     */
    public RUDPHeader() {
        header = new byte[HEADER_LENGTH];
        setVersion(VERSION);
    }

    /**
     * Builds a new header with the data from the parameter
     * @param buf byte[] with the header marshalled
     */
    public RUDPHeader(byte[] buf) {
        header = buf;
    }

    /**
     * return the marshalled representation of the header
     * @return byte[] with the header marshalled
     */
    public byte[] getHeader() {
        return header;
    }

    /**
     * Gets the SYN flag
     * @return True is the flag is set. False otherwise
     */
    public boolean isSYN() {
        return getOption(SYN_OPT);
    }

    /**
     * Gets the ACK flag
     * @return True is the flag is set. False otherwise
     */
    public boolean isACK() {
        return getOption(ACK_OPT);
    }

    /**
     * Gets the RST flag
     * @return True is the flag is set. False otherwise
     */
    public boolean isRST() {
        return getOption(RST_OPT);
    }

    /**
     * Gets the DAT flag
     * @return True is the flag is set. False otherwise
     */
    public boolean isDAT() {
        return getOption(DAT_OPT);
    }

    /**
     * Sets or usets the SYN flag.
     * @param set True to set Flase to unset
     */
    public void setSYN(boolean set) {
        setOption(SYN_OPT, set);
    }

    /**
     * Sets or usets the ACK flag.
     * @param set True to set Flase to unset
     */
    public void setACK(boolean set) {
        setOption(ACK_OPT, set);
    }

    /**
     * Sets or usets the RST flag.
     * @param set True to set Flase to unset
     */
    public void setRST(boolean set) {
        setOption(RST_OPT, set);
    }

    /**
     * Sets or usets the DAT flag.
     * @param set True to set Flase to unset
     */
    public void setDAT(boolean set) {
        setOption(DAT_OPT, set);
    }

    /**
     * Get the length field.
     * @return Length of the data between 0 and 0xFFFF
     */
    public int getLength() {
        return getShortField(LEN_OFF);
    }

    /**
     * Set the length field.
     * @param length Length of the data between 0 and 0xFFFF
     */
    public void setLength(int length) {
        setShortField(length, LEN_OFF);
    }

    /**
     * Get the SeqNumber field.
     * @return SeqNumber of this packet
     */
    public int getSeqNumber() {
        return getShortField(SEQ_OFF);
    }

    /**
     * Set the seqNumber field.
     * @param seq SeqNumber between 0 and 0xFFFF
     */
    public void setSeqNumber(int seq) {
        setShortField(seq, SEQ_OFF);
    }

    /**
     * Calculates the checksum of the header and sets it
     */
    public void setChecksum() {
        setChecksum(calculateChecksum());
    }

    /**
     * Check that the header is valid according to the protocol.
     * @return True if is valid. False if is not valid.
     */
    public boolean isValid() {
        return ((calculateChecksum() == getChecksum()) && (getVersion() == VERSION));
    }


    //--------------------------------------------------------------------------
    //                         PRIVATE METHODS
    //--------------------------------------------------------------------------
    private int calculateChecksum() {
        // TODO: Do some kind of checksum
        return 12345;
    }

    private int getChecksum() {
        return getShortField(CHK_OFF);
    }

    private void setChecksum(int check) {
        setShortField(check, CHK_OFF);
    }

    private void setVersion(byte ver) {
        header[0] = (byte)(header[0] | (ver << 4));
    }

    private byte getVersion() {
        return (byte)((header[0]&0xF0)>>4);
    }

    private void setOption(int opt, boolean value) {
        int val = value?1:0;
        byte oldByte = header[0];
        oldByte = (byte) (((0xFF7F>>opt) & oldByte) & 0x00FF);
        byte newByte = (byte) ((val<<(8-(opt+1))) | oldByte);
        header[0] = newByte;
    }

    private boolean getOption(int opt) {
        byte valByte = header[0];
        int valInt = valByte>>(8-(opt+1)) & 0x0001;
        return (valInt == 1);
    }

    private void setShortField(int value, int offset) {
        header[offset] = (byte) ((value & 0xFF00) >> 8);
        header[offset + 1] = (byte) (value & 0x00FF);
    }

    private int getShortField(int offset) {
        int result = 0;
        result = result | ((0x00FF&header[offset])<<8);
        result = result | (0x00FF&header[offset + 1]);
        return result;
    }

    /*
    public static void main(String[] args) {
        RUDPHeader p = new RUDPHeader();

        p.setDAT(true);
        p.setACK(true);
        p.setSYN(false);
        p.setRST(false);

        p.setLength(63000);
        p.setSeqNumber(61000);
        p.setChecksum();

        System.out.println("VER?" + p.getVersion());
        System.out.println("Valid? " + p.isValid());
        System.out.println("SYN?"+p.isSYN());
        System.out.println("ACK?"+p.isACK());
        System.out.println("RST?"+p.isRST());
        System.out.println("DAT?"+p.isDAT());

        System.out.println("SEQ?" + p.getSeqNumber());
        System.out.println("CHK?" + p.getChecksum());
        System.out.println("LEN?" + p.getLength());
    }
    */
}
