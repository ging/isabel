<?xml version="1.0" encoding="UTF-8"?>
<!--
 - ISABEL: A group collaboration tool for the Internet
 - Copyright (C) 2009 Agora System S.A.
 -
 - This file is part of Isabel.
 -
 - Isabel is free software: you can redistribute it and/or modify
 - it under the terms of the Affero GNU General Public License as published by
 - the Free Software Foundation, either version 3 of the License, or
 - (at your option) any later version.
 -
 - Isabel is distributed in the hope that it will be useful,
 - but WITHOUT ANY WARRANTY; without even the implied warranty of
 - MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 - Affero GNU General Public License for more details.
 -
 - You should have received a copy of the Affero GNU General Public License
 - along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 -
-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:output method="text"/>
	<xsl:param name="SITEID"/>
<!--This version applies to isabel 4.8 beta3 and upper-->
	<xsl:template match="/">
		<!--<xsl:text>Parametro: </xsl:text><xsl:value-of select="$SITEID"/>-->
		<xsl:for-each select="/EDL/site-description/SITE">
			<xsl:if test="site-identification/ID = $SITEID">
				<xsl:text>ISABEL_CONNECT_TO=</xsl:text>
                                <xsl:choose>
                                        <xsl:when test="CONNECTION-MODE/mcu/MCU_ADDRESS">
                                                <xsl:value-of select="CONNECTION-MODE/mcu/MCU_ADDRESS"/>
                                        </xsl:when>
                                        <xsl:when test="CONNECTION-MODE/unicast/PEER_ADDRESS">
                                                <xsl:value-of select="CONNECTION-MODE/unicast/PEER_ADDRESS"/>
                                        </xsl:when>
                                        <xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_ROOT">
                                                <xsl:value-of select="CONNECTION-MODE/multicast/multicast-parameters/MCAST_ROOT"/>
                                        </xsl:when>
                                </xsl:choose>
				<xsl:text disable-output-escaping="yes">
ISABEL_SESSION_PORT=</xsl:text><xsl:value-of select="/EDL/SESSION/session-info/SESSION_PORT"/>
				<xsl:text disable-output-escaping="yes">
ISABEL_SERVICE=</xsl:text>
				<xsl:choose>
				<xsl:when test="/EDL/SESSION/SERVICE/SERVICE-NAME = 'telemeeting'">
					<xsl:text disable-output-escaping="yes">meeting.act</xsl:text>
				</xsl:when>
				<xsl:when test="/EDL/SESSION/SERVICE/SERVICE-NAME = 'teleclass'">
					<xsl:text disable-output-escaping="yes">class.act</xsl:text>
				</xsl:when>
				<xsl:when test="/EDL/SESSION/SERVICE/SERVICE-NAME = 'teleconference'">
					<xsl:text disable-output-escaping="yes">conference.act</xsl:text>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="/EDL/SESSION/SERVICE/SERVICE-NAME"/>
				</xsl:otherwise>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_QUALITY=</xsl:text><xsl:value-of select="/EDL/SESSION/SERVICE/SERVICE-QUALITY"/>
			<xsl:text disable-output-escaping="yes">
ISABEL_SEND_TO_WP=</xsl:text>
			<xsl:choose>
				<xsl:when test="/EDL/SESSION/session-info/ALLOW_WATCHPOINTS = 'true'">
					<xsl:text>1</xsl:text>
				</xsl:when>
				<xsl:otherwise>
					<xsl:text>0</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_OPEN_SESSION=</xsl:text>
			<xsl:choose>
				<xsl:when test="/EDL/SESSION/session-info/OPEN_SESSION">
					<xsl:if test="/EDL/SESSION/session-info/OPEN_SESSION = 'true'">
						<xsl:text>1</xsl:text>
					</xsl:if>
					<xsl:if test="/EDL/SESSION/session-info/OPEN_SESSION = 'false'">
						<xsl:text>0</xsl:text>
					</xsl:if>	
				</xsl:when>
				<xsl:otherwise>
					<xsl:text>0</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_SESSION_ID=</xsl:text>
			<xsl:choose>
				<xsl:when test="/EDL/SESSION/ID">
					<xsl:value-of select="/EDL/SESSION/ID"/>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_GLOBAL_VAR=</xsl:text>
			<xsl:for-each select="/EDL/SESSION/SESSION_ADDITIONAL_PARAMS/*">
				<xsl:for-each select=".">
					<xsl:value-of select="name(.)"/><xsl:text> </xsl:text>
				</xsl:for-each>
			</xsl:for-each>
			<xsl:for-each select="/EDL/SESSION/SESSION_ADDITIONAL_PARAMS/*">			
				<xsl:for-each select=".">
					<xsl:text disable-output-escaping="yes">
</xsl:text>
					<xsl:value-of select="name(.)"/><xsl:text> </xsl:text>				
					<xsl:choose>
						<xsl:when test="contains(.,'true') != true">
							<xsl:text>1</xsl:text>
						</xsl:when>
						<xsl:when test="contains(.,'false') != true">
							<xsl:text>0</xsl:text>
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="."/><xsl:text></xsl:text>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:for-each>
			</xsl:for-each>
			<xsl:text disable-output-escaping="yes">
ISABEL_HOSTNAME=</xsl:text>
			<xsl:choose>
				<xsl:when test="site-identification/SITE_ADDRESS">
					<xsl:value-of select="site-identification/SITE_ADDRESS"/>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_ROLE=</xsl:text>
			<xsl:choose>
				<xsl:when test="SITE-ROLE">
					<xsl:if test="SITE-ROLE = 'Interactive'">
						<xsl:text>Interactive</xsl:text>
					</xsl:if>
					<xsl:if test="SITE-ROLE = 'interactive'">
						<xsl:text>Interactive</xsl:text>
					</xsl:if>
					<xsl:if test="SITE-ROLE = 'watchpoint'">
						<xsl:text>Watchpoint</xsl:text>
					</xsl:if>
					<xsl:if test="SITE-ROLE = 'Recorder'">
						<xsl:text>Recorder</xsl:text>
					</xsl:if>
					<xsl:if test="SITE-ROLE = 'recorder'">
						<xsl:text>Recorder</xsl:text>
					</xsl:if>
					<xsl:if test="SITE-ROLE = 'player'">
						<xsl:text>Player</xsl:text>
					</xsl:if>
					<xsl:if test="SITE-ROLE = 'noninteractive'">
						<xsl:text>Noninteractive</xsl:text>
					</xsl:if>
					<xsl:if test="SITE-ROLE = 'mcu'">
						<xsl:text>MCU</xsl:text>
					</xsl:if>
					<xsl:if test="SITE-ROLE = 'MCU'">
						<xsl:text>MCU</xsl:text>
					</xsl:if>
					<xsl:if test="SITE-ROLE = 'Antenna'">
						<xsl:text>Antenna</xsl:text>
					</xsl:if>
					<xsl:if test="SITE-ROLE = 'antenna'">
						<xsl:text>Antenna</xsl:text>
					</xsl:if>
					<xsl:if test="SITE-ROLE = 'SipGateway'">
						<xsl:text>SipGateway</xsl:text>
					</xsl:if>
					<xsl:if test="SITE-ROLE = 'sipgateway'">
                                                <xsl:text>SipGateway</xsl:text>
                                        </xsl:if>
                                        <xsl:if test="SITE-ROLE = 'flashgateway'">
                                                <xsl:text>FlashGateway</xsl:text>
                                        </xsl:if>

					<xsl:if test="SITE-ROLE = 'MediaServer'">
                                                <xsl:text>MediaServer</xsl:text>
                                        </xsl:if>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_SITE_ID=</xsl:text>
			<xsl:choose>
				<xsl:when test="site-identification/ID">
					<xsl:value-of select="site-identification/ID"/>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_PUBLIC_NAME=</xsl:text>
			<xsl:choose>
				<xsl:when test="site-identification/PUBLIC_NAME">
					<xsl:value-of select="site-identification/PUBLIC_NAME"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="site-identification/ID"/>
				</xsl:otherwise>
			</xsl:choose>
                        <xsl:text disable-output-escaping="yes">
ISABEL_PASSWD=</xsl:text>
                        <xsl:choose>
                                <xsl:when test="site-identification/ACCESS_CONTROL">
                                        <xsl:value-of select="site-identification/ACCESS_CONTROL"/>
                                </xsl:when>
                                <xsl:otherwise>
                                	<xsl:text></xsl:text>
				</xsl:otherwise>
                        </xsl:choose>			
			<xsl:text disable-output-escaping="yes">
ISABEL_USE_MULTICAST=</xsl:text>
			<xsl:choose>
				<xsl:when test="CONNECTION-MODE/multicast">
					<xsl:text>1</xsl:text>
				</xsl:when>
				<xsl:otherwise>
					<xsl:text>0</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_MCASTBW=</xsl:text>
			<xsl:choose>
				<xsl:when test="CONNECTION-MODE/multicast">
					<xsl:choose>
						<!--If there are downbandwidth and upbandwidth specified, they are used here-->
		                                <xsl:when test="connection-parameters/upanddownbandwidth/UPBANDWIDTH">
		                                        <xsl:value-of select="connection-parameters/upanddownbandwidth/UPBANDWIDTH"/>
		                                </xsl:when>
		                                <!--If only bandwidth has been set, it is used both as downbandwidth and upbandwidth-->
                		                <xsl:when test="connection-parameters/BANDWIDTH">
                                		        <xsl:value-of select="connection-parameters/BANDWIDTH"/>
		                                </xsl:when>
		                                <!--If no bandwidth has been set, session quality is used-->
		                                <xsl:when test="/EDL/SESSION/SERVICE/SERVICE-QUALITY">
                		                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '10M'">
                                		                <xsl:text>10000</xsl:text>
                                        		</xsl:if>
		                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '2M'">
		                                                <xsl:text>2000</xsl:text>
		                                        </xsl:if>
		                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '1M'">
		                                                <xsl:text>1000</xsl:text>
		                                        </xsl:if>
		                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '512K'">
		                                                <xsl:text>512</xsl:text>
		                                        </xsl:if>
		                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '384K'">
		                                                <xsl:text>384</xsl:text>
		                                        </xsl:if>
		                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '300K/1M'">
		                                                <xsl:text>300</xsl:text>
		                                        </xsl:if>
		                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '256K'">
		                                                <xsl:text>256</xsl:text>
		                                        </xsl:if>
		                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '128/256K'">
		                                                <xsl:text>128</xsl:text>
		                                        </xsl:if>
		                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '128K'">
		                                                <xsl:text>128</xsl:text>
		                                        </xsl:if>
		                                </xsl:when>
					</xsl:choose>				
				</xsl:when>
				<xsl:otherwise>
                                        <xsl:text>0</xsl:text>
                                </xsl:otherwise>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_USE_FLOWSERVER=</xsl:text>
			<xsl:choose>
					<xsl:when test="CONNECTION-MODE/mcu/MCU_ADDRESS">
						<xsl:text>1</xsl:text>
					</xsl:when>
					<xsl:when test="CONNECTION-MODE/unicast/PEER_ADDRESS">
						<xsl:text>1</xsl:text>
					</xsl:when>
					<xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_ROOT =  '%ROOT_NODE%'">
						<xsl:text>1</xsl:text>
					</xsl:when>
					<xsl:otherwise>
						<xsl:text>0</xsl:text>
					</xsl:otherwise>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_FSUPBW=</xsl:text>
			<xsl:choose>
				<!--If there are downbandwidth and upbandwidth specified, they are used here-->
				<xsl:when test="connection-parameters/upanddownbandwidth/UPBANDWIDTH">
					<xsl:value-of select="connection-parameters/upanddownbandwidth/UPBANDWIDTH"/>
				</xsl:when>
				<!--If only bandwidth has been set, it is used both as downbandwidth and upbandwidth-->
				<xsl:when test="connection-parameters/BANDWIDTH">
					<xsl:value-of select="connection-parameters/BANDWIDTH"/>
				</xsl:when>
				<!--If no bandwidth has been set, session quality is used-->
                                <xsl:when test="/EDL/SESSION/SERVICE/SERVICE-QUALITY">
                                	<xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '10M'">
                                        	<xsl:text>10000</xsl:text>
                                        </xsl:if>
                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '2M'">
                                        	<xsl:text>2000</xsl:text>
                                        </xsl:if>
                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '1M'">
                                        	<xsl:text>1000</xsl:text>
                                        </xsl:if>
                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '512K'">
                                        	<xsl:text>512</xsl:text>
                                        </xsl:if>
                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '384K'">
		                                    <xsl:text>384</xsl:text>
		                                </xsl:if>
		                                <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '300K/1M'">
		                                    <xsl:text>300</xsl:text>
		                                </xsl:if>
                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '256K'">
                                        	<xsl:text>256</xsl:text>
                                        </xsl:if>
                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '128/256K'">
                                        	<xsl:text>128</xsl:text>
                                        </xsl:if>
                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '128K'">
                                        	<xsl:text>128</xsl:text>
                                        </xsl:if>
                                </xsl:when>
				<xsl:otherwise>
					<xsl:text>-1</xsl:text>
				</xsl:otherwise>
			</xsl:choose>

			<xsl:text disable-output-escaping="yes">
ISABEL_FSDOWNBW=</xsl:text>
			<xsl:choose>
				<!--If there are downbandwidth and upbandwidth specified, they are used here-->
				<xsl:when test="connection-parameters/upanddownbandwidth/DOWNBANDWIDTH">
					<xsl:value-of select="connection-parameters/upanddownbandwidth/DOWNBANDWIDTH"/>
				</xsl:when>
				<!--If only bandwidth has been set, it is used both as downbandwidth and upbandwidth-->
				<xsl:when test="connection-parameters/BANDWIDTH">
					<xsl:value-of select="connection-parameters/BANDWIDTH"/>
				</xsl:when>
				<!--If no bandwidth has been set, session quality is used-->
                                <xsl:when test="/EDL/SESSION/SERVICE/SERVICE-QUALITY">
                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '10M'">
                                                <xsl:text>10000</xsl:text>
                                        </xsl:if>
                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '2M'">
                                                <xsl:text>2000</xsl:text>
                                        </xsl:if>
                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '1M'">
                                                <xsl:text>1000</xsl:text>
                                        </xsl:if>
                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '512K'">
                                                <xsl:text>512</xsl:text>
                                        </xsl:if>
                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '384K'">
		                                        <xsl:text>384</xsl:text>
		                                </xsl:if>
		                                <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '300K/1M'">
		                                        <xsl:text>300</xsl:text>
		                                </xsl:if>
                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '256K'">
                                                <xsl:text>256</xsl:text>
                                        </xsl:if>
                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '128/256K'">
                                                <xsl:text>256</xsl:text>
                                        </xsl:if>
                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '128K'">
                                                <xsl:text>128</xsl:text>
                                        </xsl:if>
                                </xsl:when>
				<xsl:otherwise>
					<xsl:text>-1</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_MCAST_TTL=</xsl:text>
			<xsl:choose>
				<xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_TTL">
					<xsl:value-of select="CONNECTION-MODE/multicast/multicast-parameters/MCAST_TTL"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:text>5</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_MCAST_AUDIO_GROUP=</xsl:text>
			<xsl:choose>
				<xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_AUDIO_GROUP">
					<xsl:value-of select="CONNECTION-MODE/multicast/multicast-parameters/MCAST_AUDIO_GROUP"/>
				</xsl:when>
				<xsl:otherwise>
                                        <xsl:text>239.255.6.1</xsl:text>
                                </xsl:otherwise>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_MCAST_VUMETER_GROUP=</xsl:text>
			<xsl:choose>
				<xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_VUMETER_GROUP">	
					<xsl:value-of select="CONNECTION-MODE/multicast/multicast-parameters/MCAST_VUMETER_GROUP"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:text>239.255.6.1</xsl:text>
                                </xsl:otherwise>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_MCAST_VIDEO_GROUP=</xsl:text>
			<xsl:choose>
				<xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_VIDEO_GROUP">
					<xsl:value-of select="CONNECTION-MODE/multicast/multicast-parameters/MCAST_VIDEO_GROUP"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:text>239.255.6.1</xsl:text>
                                </xsl:otherwise>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_MCAST_FTP_GROUP=</xsl:text>
			<xsl:choose>
				<xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_FTP_GROUP">
					<xsl:value-of select="CONNECTION-MODE/multicast/multicast-parameters/MCAST_FTP_GROUP"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:text>239.255.6.1</xsl:text>
                                </xsl:otherwise>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_MCAST_POINTER_GROUP=</xsl:text>
			<xsl:choose>
				<xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_POINTER_GROUP">
					<xsl:value-of select="CONNECTION-MODE/multicast/multicast-parameters/MCAST_POINTER_GROUP"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:text>239.255.6.1</xsl:text>
                                </xsl:otherwise>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_MCAST_SHDISPLAY_GROUP=</xsl:text>
			<xsl:choose>
				<xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_APPSH_GROUP">	
					<xsl:value-of select="CONNECTION-MODE/multicast/multicast-parameters/MCAST_APPSH_GROUP"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:text>239.255.6.1</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_NETWORK_ACCESS=</xsl:text>
			<xsl:choose>
				<xsl:when test="NETWORK-ACCESS/Ethernet">
					<xsl:text>Ethernet</xsl:text>
				</xsl:when>
				<xsl:when test="NETWORK-ACCESS/ISDN">
					<xsl:text>ISDN</xsl:text>
				</xsl:when>
				<xsl:when test="NETWORK-ACCESS/modem">
					<xsl:text>modem</xsl:text>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_ISDN_ROLE=</xsl:text>
				<xsl:choose>
					<xsl:when test="NETWORK-ACCESS/ISDN/ISDN-parameters/ISDN-caller">
						<xsl:text>Caller</xsl:text>
					</xsl:when>
					<xsl:when test="NETWORK-ACCESS/ISDN/ISDN-parameters/ISDN-called">
						<xsl:text>Called</xsl:text>
					</xsl:when>
					<xsl:otherwise>
						<xsl:text>Caller</xsl:text>
					</xsl:otherwise>
				</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_ISDN_NUMBERS=</xsl:text>
				<xsl:choose>
					<xsl:when test="NETWORK-ACCESS/ISDN/ISDN-parameters/ISDN-caller/ISDN_PHONE_MSN">
						<xsl:for-each select="NETWORK-ACCESS/ISDN/ISDN-parameters/ISDN-caller/ISDN_PHONE_MSN">
							<xsl:value-of select="."/><xsl:text> </xsl:text>
							<xsl:value-of select="../ISDN_PHONE_NUMBER[position()]"/><xsl:text> </xsl:text>
						</xsl:for-each>
					</xsl:when>
					<!--<xsl:when test="NETWORK-ACCESS/ISDN/ISDN-parameters/ISDN-caller/ISDN_PHONE_NUMBER">
						<xsl:for-each select="NETWORK-ACCESS/ISDN/ISDN-parameters/ISDN-caller/ISDN_PHONE_NUMBER">
							<xsl:value-of select="."/><xsl:text> </xsl:text>
							<xsl:value-of select="../ISDN_PHONE_NUMBER[position()]"/><xsl:text> </xsl:text>
						</xsl:for-each>
					</xsl:when>-->
				</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_ISDN_CHANNELS=</xsl:text>
			<xsl:choose>
				<xsl:when test="NETWORK-ACCESS/ISDN/ISDN-parameters/ISDN-caller/ISDN_CHANNELS">
					<xsl:value-of select="NETWORK-ACCESS/ISDN/ISDN-parameters/ISDN-caller/ISDN_CHANNELS"/>
				</xsl:when>
				<xsl:when test="NETWORK-ACCESS/ISDN/ISDN-parameters/ISDN-called/ISDN_CHANNELS">
					<xsl:value-of select="NETWORK-ACCESS/ISDN/ISDN-parameters/ISDN-called/ISDN_CHANNELS"/>
				</xsl:when>				
				<xsl:otherwise>
					<xsl:text>1</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_ISDN_BOUND=</xsl:text>
			<xsl:choose>
				<xsl:when test="connection-parameters/BANDWIDTH">
					<xsl:value-of select="ceiling(connection-parameters/BANDWIDTH div 64)"/>
				</xsl:when>
				<xsl:when test="connection-parameters/upanddownbandwidth">
					<xsl:if test="connection-parameters/upanddownbandwidth/DOWNBANDWIDTH >= connection-parameters/upanddownbandwidth/UPBANDWIDTH">									<xsl:value-of select="ceiling(connection-parameters/upanddownbandwidth/DOWNBANDWIDTH div 64)"/>
					</xsl:if>
					<xsl:if test="connection-parameters/upanddownbandwidth/UPNBANDWIDTH >= connection-parameters/upanddownbandwidth/DOWNBANDWIDTH">								<xsl:value-of select="ceiling(connection-parameters/upanddownbandwidth/UPBANDWIDTH div 64)"/>
					</xsl:if>
				</xsl:when>
				<xsl:when test="/EDL/SESSION/SERVICE/SERVICE-QUALITY">
					<xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '10M'">
                                        	<xsl:value-of select="ceiling(10000 div 64)"/>
                                	</xsl:if>
					<xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '2M'">
                                                <xsl:value-of select="ceiling(2000 div 64)"/>
                                        </xsl:if>
					<xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '1M'">
                                                <xsl:value-of select="ceiling(1000 div 64)"/>
                                        </xsl:if>
					<xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '512K'">
                                                <xsl:value-of select="ceiling(512 div 64)"/>
                                        </xsl:if>
                    <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '384K'">
                                              <xsl:value-of select="ceiling(384 div 64)"/>
		                               </xsl:if>
		            <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '300K/1M'">
		                                      <xsl:value-of select="ceiling(1000 div 64)"/>
		                               </xsl:if>
					<xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '256K'">
                                                <xsl:value-of select="ceiling(256 div 64)"/>
                                        </xsl:if>
					<xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '128/256K'">
                                                <xsl:value-of select="ceiling(256 div 64)"/>
                                        </xsl:if>
					<xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '128'">
                                                <xsl:value-of select="ceiling(128 div 64)"/>
                                        </xsl:if>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_CTRL_DISPLAY=</xsl:text>
			<xsl:choose>
				<xsl:when test="media-control/DEFAULT_CTRL">
					<xsl:value-of select="media-control/DEFAULT_CTRL"/>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_AUDIO_CTRL_DISPLAY=</xsl:text>
			<xsl:choose>
				<xsl:when test="media-control/AUDIO_CTRL">
					<xsl:value-of select="media-control/AUDIO_CTRL"/>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_POINTER_CTRL_DISPLAY=</xsl:text>
			<xsl:choose>
				<xsl:when test="media-control/DEFAULT_CTRL">
					<xsl:value-of select="media-control/AUDIO_CTRL"/>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_SCRIPTS_CTRL_DISPLAY=</xsl:text>
			<xsl:choose>
				<xsl:when test="media-control/DEFAULT_CTRL">
					<xsl:value-of select="media-control/AUDIO_CTRL"/>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_SLIDES_CTRL_DISPLAY=</xsl:text>
			<xsl:choose>
				<xsl:when test="media-control/DEFAULT_CTRL">
					<xsl:value-of select="media-control/AUDIO_CTRL"/>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_SLIDESFTP_CTRL_DISPLAY=</xsl:text>
			<xsl:choose>
				<xsl:when test="media-control/DEFAULT_CTRL">
					<xsl:value-of select="media-control/AUDIO_CTRL"/>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_VIDEO_CTRL_DISPLAY=</xsl:text>
			<xsl:choose>
				<xsl:when test="media-control/VIDEO_CTRL">
					<xsl:value-of select="media-control/VIDEO_CTRL"/>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_VMWARE_CTRL_DISPLAY=</xsl:text>
			<xsl:choose>
				<xsl:when test="media-control/APP_SHARING_CTRL">
					<xsl:value-of select="media-control/APP_SHARING_CTRL"/>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_XSERVER_CTRL_DISPLAY=</xsl:text>
			<xsl:choose>
				<xsl:when test="media-control/APP_SHARING_CTRL">
					<xsl:value-of select="media-control/APP_SHARING_CTRL"/>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
ISABEL_IROUTER_FEC=</xsl:text>
			<xsl:choose>
				<xsl:when test="connection-parameters/PROTECTION/parity">
					<xsl:value-of select="connection-parameters/PROTECTION/parity/n"/><xsl:text>,</xsl:text><xsl:value-of select="connection-parameters/PROTECTION/parity/k"/>
				</xsl:when>
			</xsl:choose>	
			<xsl:text disable-output-escaping="yes">
ISABEL_LOCAL_VAR=</xsl:text>
			<xsl:for-each select="SITE_ADDITIONAL_PARAMS/*">
				<xsl:for-each select=".">
					<xsl:value-of select="name(.)"/><xsl:text> </xsl:text>
				</xsl:for-each>
			</xsl:for-each>
			<xsl:for-each select="SITE_ADDITIONAL_PARAMS/*">
				<xsl:text disable-output-escaping="yes">
</xsl:text>
				<xsl:value-of select="name(.)"/><xsl:text>=</xsl:text>
				<xsl:choose>
					<xsl:when test="contains(.,'true') != true">
						<xsl:text>1</xsl:text>
					</xsl:when>
					<xsl:when test="contains(.,'false') != true">
						<xsl:text>0</xsl:text>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="."/>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:for-each>
			</xsl:if>
		</xsl:for-each>
<xsl:text disable-output-escaping="yes">
</xsl:text>
	</xsl:template>
</xsl:stylesheet>
