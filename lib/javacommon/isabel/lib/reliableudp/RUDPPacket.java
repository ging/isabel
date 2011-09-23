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
 * This class represents a datagram packet
 * @author Fernando Escribano
 */
public class RUDPPacket {

    public static final int RUDP_MTU = 2048;
    
    /**
     * Byte array holding the data
     */
    private byte[] data;

    /**
     * Offset in the data array where the data actually starts
     */
    private int offset = 0;

    /**
     * Header of the datagram
     */
    RUDPHeader header;


    /**
     * Builds a RUDP Data packet
     * @param buf byte[] holding the data to be sent
     * @param length length of the data
     */
    public RUDPPacket(byte[] buf, int length) {
        this(buf, 0, length, true);
    }

    /**
     * Builds a RUDP Data packet
     * @param buf byte[] holding the data to be sent
     * @param offset Offset in the data array where the data starts.
     * @param length length length of the data
     */
    public RUDPPacket(byte[] buf, int offset, int length) {
        this(buf, offset, length, true);
    }

    /**
     * Builds a RUDP packet
     * @param buf byte[] holding the data to be sent
     * @param offset Offset in the data array where the data starts.
     * @param length length length of the data
     */
    public RUDPPacket(byte[] buf, int offset, int length, boolean dat) {
        header = new RUDPHeader();
        data = new byte[length];
        System.arraycopy(buf, offset, data, 0, length);
        header.setLength(length);
        header.setDAT(dat);
        this.offset = 0;
    }

    /**
     * Build a RUDPPacket from a buffer containing the header and the data
     * @param buf
     */
    RUDPPacket(byte[] buf) {
        header = new RUDPHeader(buf);
        data = buf;
        offset = RUDPHeader.HEADER_LENGTH;
    }

    /**
     * Returns the data array. It may have some offset.
     * @return byte[] holdign the data.
     */
    public byte[] getData() {
        return data;
    }

    /**
     * Gets the length of the data
     * @return data length
     */
    public int getLength() {
        return header.getLength();
    }

    /**
     * Gets the offset where the data starts in the data array
     * @return offset to the data in the data array.
     */
    public int getOffset() {
        return offset;
    }

    @Override
    public String toString() {
        String result = "";
        result = result.concat("Seq#" + header.getSeqNumber() + ":");
        result = result.concat(header.isSYN()?"SYN:":"");
        result = result.concat(header.isACK()?"ACK:":"");
        result = result.concat(header.isRST()?"RST:":"");
        result = result.concat(header.isDAT()?"DAT:":"");
        if (header.isDAT()) {
            result = result.concat(new String(getData(), getOffset(), getLength()));
        }
        return result;

    }

    /**
     * Returns the packet as a byte[]
     * @return byte[] holding the header and the data.
     */
    byte[] marshall() {
        byte[] buf = new byte[getLength() + RUDPHeader.HEADER_LENGTH];
        System.arraycopy(header.getHeader(), 0, buf, 0, RUDPHeader.HEADER_LENGTH);
        System.arraycopy(getData(), getOffset(), buf, RUDPHeader.HEADER_LENGTH, getLength());
        return buf;
    }

    //--------------------------------------------------------------------------
    //                             FACTORY METHODS
    //--------------------------------------------------------------------------

    static RUDPPacket createAck(int seq) {
        RUDPPacket packet = new RUDPPacket(new byte[1], 0, 0, false);
        packet.header.setACK(true);
        packet.header.setSeqNumber(seq);
        packet.header.setChecksum();
        return packet;
    }

    static RUDPPacket createSyn(int seq) {
        RUDPPacket packet = new RUDPPacket(new byte[1], 0, 0, false);
        packet.header.setSYN(true);
        packet.header.setSeqNumber(seq);
        packet.header.setChecksum();
        return packet;

    }

    static RUDPPacket createRst(int seq) {
        RUDPPacket packet = new RUDPPacket(new byte[1], 0, 0, false);
        packet.header.setRST(true);
        packet.header.setSeqNumber(seq);
        packet.header.setChecksum();
        return packet;
    }
}
