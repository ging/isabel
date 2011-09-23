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
 * A class that saves packet stats
 * @author Fernando Escribano
 */
class Stats {

    private int dataPacketsSent = 0;
    private int dataPacketsRcvd = 0;
    private int ackPacketsSent = 0;
    private int ackPacketsRcvd = 0;
    private int retransmissionSent = 0;

    public void packetSent(RUDPPacket packet) {
        if (packet.header.isACK()) {
            ackPacketsSent++;
        }
        else if (packet.header.isDAT()) {
            dataPacketsSent++;
        }
    }

    public void packetRcvd(RUDPPacket packet) {
        if (packet.header.isACK()) {
            ackPacketsRcvd++;
        }
        else if (packet.header.isDAT()) {
            dataPacketsRcvd++;
        }
    }

    public void retransmissionSent() {
        retransmissionSent++;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder(200);
        sb.append("Data Packets sent:" ).append(dataPacketsSent).append('\n');
        sb.append("ACK Packets sent:" ).append(ackPacketsSent).append('\n');
        sb.append("Data Packets rcvd:" ).append(dataPacketsRcvd).append('\n');
        sb.append("ACK Packets rcvd:" ).append(ackPacketsRcvd).append('\n');
        sb.append("Retransmissions sent:" ).append(retransmissionSent).append('\n');
        return sb.toString();
    }
}
