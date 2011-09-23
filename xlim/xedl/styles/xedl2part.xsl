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
	<xsl:template match="/">
		<xsl:for-each select="EDL/site-description/SITE">
			<xsl:if test="SITE-ROLE = 'Interactive' or SITE-ROLE = 'interactive' or SITE-ROLE = 'MediaServer'"> 

			<xsl:value-of select="site-identification/ID"/><xsl:text> -name "</xsl:text>
			<xsl:choose>
				<xsl:when test="site-identification/PUBLIC_NAME">
					<xsl:value-of select="site-identification/PUBLIC_NAME"/><xsl:text>"</xsl:text>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="site-identification/ID"/><xsl:text>"</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
			<xsl:choose>
				<xsl:when test="CONNECTION-MODE/mcu/MCU_ADDRESS">
					<!--<xsl:choose>
                                		<xsl:when test="site-identification/PUBLIC_NAME">
                                        		<xsl:text>"</xsl:text><xsl:value-of select="site-identification/PUBLIC_NAME"/><xsl:text>"</xsl:text>
                                		</xsl:when>
                                		<xsl:otherwise>
                                        		<xsl:text>"</xsl:text><xsl:value-of select="site-identification/ID"/><xsl:text>"</xsl:text>
                                		</xsl:otherwise>
                        		</xsl:choose>-->
					<xsl:text> -flowserver "</xsl:text><xsl:value-of select="CONNECTION-MODE/mcu/MCU_ADDRESS"/><xsl:text>"</xsl:text>
					<xsl:choose>
						<!--If there are downbandwidth and upbandwidth specified, they are used here-->
						<xsl:when test="connection-parameters/upanddownbandwidth/DOWNBANDWIDTH">
							<xsl:text> -fsdownbw </xsl:text><xsl:value-of select="connection-parameters/upanddownbandwidth/DOWNBANDWIDTH"/>
						</xsl:when>
						<xsl:when test="connection-parameters/upanddownbandwidth/UPBANDWIDTH">
							<xsl:text> -fsupbw </xsl:text><xsl:value-of select="connection-parameters/upanddownbandwidth/UPBANDWIDTH"/>
						</xsl:when>
						<!--If only bandwidth has been set, it is used both as downbandwidth and upbandwidth-->
						<xsl:when test="connection-parameters/BANDWIDTH">
							<xsl:text> -fsdownbw </xsl:text><xsl:value-of select="connection-parameters/BANDWIDTH"/><xsl:text> -fsupbw </xsl:text><xsl:value-of select="connection-parameters/BANDWIDTH"/>
						</xsl:when>
						<!--If no bandwidth has been set, session quality is used-->
						<xsl:when test="/EDL/SESSION/SERVICE/SERVICE-QUALITY">
							<xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '10M'">
                                                        	<xsl:text> -fsdownbw 10000 -fsupbw 10000</xsl:text>
							</xsl:if>
							<xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '2M'">
                                                                <xsl:text> -fsdownbw 2000 -fsupbw 2000</xsl:text>
                                                        </xsl:if>
							<xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '1M'">
                                                                <xsl:text> -fsdownbw 1000 -fsupbw 1000</xsl:text>
                                                        </xsl:if>
							<xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '512K'">
                                                                <xsl:text> -fsdownbw 512 -fsupbw 512</xsl:text>
                                                        </xsl:if>
                            <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '384K'">
                                                                <xsl:text> -fsdownbw 384 -fsupbw 384</xsl:text>
                                                        </xsl:if>
                            <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '300K/1M'">
                                                                <xsl:text> -fsdownbw 1000 -fsupbw 300</xsl:text>
                                                        </xsl:if>
							<xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '256K'">
                                                                <xsl:text> -fsdownbw 256 -fsupbw 256</xsl:text>
                                                        </xsl:if>
							<xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '128/256K'">
                                                                <xsl:text> -fsdownbw 256 -fsupbw 128</xsl:text>
                                                        </xsl:if>
							<xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '128K'">
                                                                <xsl:text> -fsdownbw 128 -fsupbw 128</xsl:text>
                                                        </xsl:if>
                                                </xsl:when>
						<xsl:otherwise>
							<xsl:text> -fsdownbw -1 -fsupbw -1</xsl:text>
						</xsl:otherwise>						
					</xsl:choose>
				</xsl:when>
				<xsl:when test="CONNECTION-MODE/multicast">
					<xsl:text> -multicast 1 -mcastbw -1</xsl:text>
				</xsl:when>
			</xsl:choose>
			<xsl:choose>
			<xsl:when test="CONNECTION-MODE/unicast/PEER_ADDRESS">
					<xsl:text> -flowserver "</xsl:text><xsl:value-of select="CONNECTION-MODE/unicast/PEER_ADDRESS"/><xsl:text>"</xsl:text>
					<xsl:choose>
						<!--If there are downbandwidth and upbandwidth specified, they are used here-->
						<xsl:when test="connection-parameters/upanddownbandwidth/DOWNBANDWIDTH">
							<xsl:text> -fsdownbw </xsl:text><xsl:value-of select="connection-parameters/upanddownbandwidth/DOWNBANDWIDTH"/>
						</xsl:when>
						<xsl:when test="connection-parameters/upanddownbandwidth/UPBANDWIDTH">
							<xsl:text> -fsupbw </xsl:text><xsl:value-of select="connection-parameters/upanddownbandwidth/UPBANDWIDTH"/>
						</xsl:when>
						<!--If only bandwidth has been set, it is used both as downbandwidth and upbandwidth-->
						<xsl:when test="connection-parameters/BANDWIDTH">
							<xsl:text> -fsdownbw </xsl:text><xsl:value-of select="connection-parameters/BANDWIDTH"/><xsl:text> -fsupbw </xsl:text><xsl:value-of select="connection-parameters/BANDWIDTH"/>
						</xsl:when>
						<!--If no bandwidth has been set, session quality is used-->
                                                <xsl:when test="/EDL/SESSION/SERVICE/SERVICE-QUALITY">
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '10M'">
                                                                <xsl:text> -fsdownbw 10000 -fsupbw 10000</xsl:text>
                                                        </xsl:if>
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '2M'">
                                                                <xsl:text> -fsdownbw 2000 -fsupbw 2000</xsl:text>
                                                        </xsl:if>
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '1M'">
                                                                <xsl:text> -fsdownbw 1000 -fsupbw 1000</xsl:text>
                                                        </xsl:if>
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '512K'">
                                                                <xsl:text> -fsdownbw 512 -fsupbw 512</xsl:text>
                                                        </xsl:if>
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '384K'">
                                                                <xsl:text> -fsdownbw 384 -fsupbw 384</xsl:text>
                                                        </xsl:if>
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '300K/1M'">
                                                                <xsl:text> -fsdownbw 1000 -fsupbw 300</xsl:text>
                                                        </xsl:if>
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '256K'">
                                                                <xsl:text> -fsdownbw 256 -fsupbw 256</xsl:text>
                                                        </xsl:if>
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '128/256K'">
                                                                <xsl:text> -fsdownbw 256 -fsupbw 128</xsl:text>
                                                        </xsl:if>
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '128K'">
                                                                <xsl:text> -fsdownbw 128 -fsupbw 128</xsl:text>
                                                        </xsl:if>
                                                </xsl:when>						
						<xsl:otherwise>
							<xsl:text> -fsdownbw -1 -fsupbw -1</xsl:text>
						</xsl:otherwise>						
					</xsl:choose>
				</xsl:when>
				<xsl:when test="CONNECTION-MODE/multicast">
					<xsl:text> -multicast 1 -mcastbw -1</xsl:text>
				</xsl:when>
			</xsl:choose>
			<xsl:choose>
			<xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_ROOT">
					<xsl:text> -flowserver "</xsl:text><xsl:value-of select="CONNECTION-MODE/multicast/multicast-parameters/MCAST_ROOT"/><xsl:text>"</xsl:text>
					<xsl:choose>
						<!--If there are downbandwidth and upbandwidth specified, they are used here-->
						<xsl:when test="connection-parameters/upanddownbandwidth/DOWNBANDWIDTH">
							<xsl:text> -fsdownbw </xsl:text><xsl:value-of select="connection-parameters/upanddownbandwidth/DOWNBANDWIDTH"/>
						</xsl:when>
						<xsl:when test="connection-parameters/upanddownbandwidth/UPBANDWIDTH">
							<xsl:text> -fsupbw </xsl:text><xsl:value-of select="connection-parameters/upanddownbandwidth/UPBANDWIDTH"/>
						</xsl:when>
						<!--If only bandwidth has been set, it is used both as downbandwidth and upbandwidth-->
						<xsl:when test="connection-parameters/BANDWIDTH">
							<xsl:text> -fsdownbw </xsl:text><xsl:value-of select="connection-parameters/BANDWIDTH"/><xsl:text> -fsupbw </xsl:text><xsl:value-of select="connection-parameters/BANDWIDTH"/>
						</xsl:when>
						<!--If no bandwidth has been set, session quality is used-->
                                                <xsl:when test="/EDL/SESSION/SERVICE/SERVICE-QUALITY">
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '10M'">
                                                                <xsl:text> -fsdownbw 10000 -fsupbw 10000</xsl:text>
                                                        </xsl:if>
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '2M'">
                                                                <xsl:text> -fsdownbw 2000 -fsupbw 2000</xsl:text>
                                                        </xsl:if>
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '1M'">
                                                                <xsl:text> -fsdownbw 1000 -fsupbw 1000</xsl:text>
                                                        </xsl:if>
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '512K'">
                                                                <xsl:text> -fsdownbw 512 -fsupbw 512</xsl:text>
                                                        </xsl:if>
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '384K'">
                                                                <xsl:text> -fsdownbw 384 -fsupbw 384</xsl:text>
                                                        </xsl:if>
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '300K/1M'">
                                                                <xsl:text> -fsdownbw 1000 -fsupbw 300</xsl:text>
                                                        </xsl:if>
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '256K'">
                                                                <xsl:text> -fsdownbw 256 -fsupbw 256</xsl:text>
                                                        </xsl:if>
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '128/256K'">
                                                                <xsl:text> -fsdownbw 256 -fsupbw 128</xsl:text>
                                                        </xsl:if>
                                                        <xsl:if test="/EDL/SESSION/SERVICE/SERVICE-QUALITY = '128K'">
                                                                <xsl:text> -fsdownbw 128 -fsupbw 128</xsl:text>
                                                        </xsl:if>
                                                </xsl:when>
						<xsl:otherwise>
							<xsl:text> -fsdownbw -1 -fsupbw -1</xsl:text>
						</xsl:otherwise>						
					</xsl:choose>
				</xsl:when>
				<xsl:when test="CONNECTION-MODE/multicast">
					<xsl:text> -multicast 1 -mcastbw -1</xsl:text>
				</xsl:when>
			</xsl:choose>


			<xsl:text disable-output-escaping="yes">
</xsl:text>
			</xsl:if>
		</xsl:for-each>
	</xsl:template>
</xsl:stylesheet>

			
			
			
			
			
			
			<!--<xsl:choose>
				<xsl:when test="site-identification/ACCESS_CONTROL">
					<xsl:text disable-output-escaping="yes">
ACCESS_CONTROL:</xsl:text>
					<xsl:value-of select="site-identification/ACCESS_CONTROL"/>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
PUBLIC_NAME:</xsl:text>
			<xsl:value-of select="site-identification/PUBLIC_NAME"/>
			<xsl:text disable-output-escaping="yes">
SITE_ADDRESS:</xsl:text>
			<xsl:value-of select="site-identification/SITE_ADDRESS"/>
			<xsl:text disable-output-escaping="yes">
ROLE:</xsl:text>
			<xsl:value-of select="SITE-ROLE"/>
			<xsl:choose>
				<xsl:when test="CONNECTION-MODE/mcu/MCU_ADDRESS">
					<xsl:text disable-output-escaping="yes">
CONNECTION_MODE: mcu
MCU_ADDRESS: </xsl:text>
					<xsl:value-of select="CONNECTION-MODE/mcu/MCU_ADDRESS"/>
				</xsl:when>
			</xsl:choose>
			<xsl:choose>
				<xsl:when test="CONNECTION-MODE/unicast/PEER_ADDRESS">
					<xsl:text disable-output-escaping="yes">
CONNECTION_MODE: unicast
PEER_ADDRESS: </xsl:text>
					<xsl:value-of select="CONNECTION-MODE/unicast/PEER_ADDRESS"/>
				</xsl:when>
			</xsl:choose>
			<xsl:choose>
				<xsl:when test="CONNECTION-MODE/multicast">
					<xsl:text disable-output-escaping="yes">
CONNECTION_MODE: multicast</xsl:text>
				<xsl:choose>
					<xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_TTL">
						<xsl:text disable-output-escaping="yes">
MCAST_TTL: </xsl:text>
					<xsl:value-of select="CONNECTION-MODE/multicast/multicast-parameters/MCAST_TTL"/>
					</xsl:when>
				</xsl:choose>
				<xsl:choose>
					<xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_AUDIO_GROUP">
						<xsl:text disable-output-escaping="yes">
MCAST_AUDIO_GROUP: </xsl:text>
					<xsl:value-of select="CONNECTION-MODE/multicast/multicast-parameters/MCAST_AUDIO_GROUP"/>
					</xsl:when>
				</xsl:choose>
				<xsl:choose>
					<xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_VIDEO_GROUP">
						<xsl:text disable-output-escaping="yes">
MCAST_VIDEO_GROUP: </xsl:text>
					<xsl:value-of select="CONNECTION-MODE/multicast/multicast-parameters/MCAST_VIDEO_GROUP"/>
					</xsl:when>
				</xsl:choose>
				<xsl:choose>
					<xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_FTP_GROUP">
						<xsl:text disable-output-escaping="yes">
MCAST_FTP_GROUP: </xsl:text>
					<xsl:value-of select="CONNECTION-MODE/multicast/multicast-parameters/MCAST_FTP_GROUP"/>
					</xsl:when>
				</xsl:choose>
				<xsl:choose>
					<xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_POINTER_GROUP">
						<xsl:text disable-output-escaping="yes">
MCAST_POINTER_GROUP: </xsl:text>
					<xsl:value-of select="CONNECTION-MODE/multicast/multicast-parameters/MCAST_POINTER_GROUP"/>
					</xsl:when>
				</xsl:choose>
				<xsl:choose>
					<xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_CTRLWP_GROUP">	
						<xsl:text disable-output-escaping="yes">
MCAST_CTRLWP_GROUP: </xsl:text>
					<xsl:value-of select="CONNECTION-MODE/multicast/multicast-parameters/MCAST_CTRLWP_GROUP"/>
					</xsl:when>
				</xsl:choose>
				<xsl:choose>
					<xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_APPSH_GROUP">	
						<xsl:text disable-output-escaping="yes">
MCAST_APPSH_GROUP: </xsl:text>
					<xsl:value-of select="CONNECTION-MODE/multicast/multicast-parameters/MCAST_APPSH_GROUP"/>
					</xsl:when>
				</xsl:choose>
				</xsl:when>
			</xsl:choose>
			<xsl:choose>
				<xsl:when test="connection-parameters/BANDWIDTH">
					<xsl:text disable-output-escaping="yes">
BANDWIDTH: </xsl:text>
					<xsl:value-of select="connection-parameters/BANDWIDTH"/>
				</xsl:when>
				<xsl:when test="connection-parameters/upanddownbandwidth/UPBANDWIDTH">
					<xsl:text disable-output-escaping="yes">
UPBANDWIDTH: </xsl:text>
					<xsl:value-of select="connection-parameters/upanddownbandwidth/UPBANDWIDTH"/>
					<xsl:text disable-output-escaping="yes">
DOWNBANDWIDTH: </xsl:text>
					<xsl:value-of select="connection-parameters/upanddownbandwidth/DOWNBANDWIDTH"/>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
NETWORK_ACCESS: </xsl:text>
			<xsl:choose>
				<xsl:when test="NETWORK-ACCESS/Ethernet">
					<xsl:text disable-output-escaping="yes">Ethernet</xsl:text>
				</xsl:when>
			</xsl:choose>
			<xsl:choose>
				<xsl:when test="CONTROL-CAPABILITIES">
					<xsl:text disable-output-escaping="yes">
CONTROL_CAPABILITIES:</xsl:text>
					<xsl:value-of select="CONTROL-CAPABILITIES"/>
				</xsl:when>
			</xsl:choose>
			<xsl:choose>
				<xsl:when test="media-control/DEFAULT_CTRL">
					<xsl:text disable-output-escaping="yes">
DEFAULT_CTRL:</xsl:text>
					<xsl:value-of select="media-control/DEFAULT_CTRL"/>
				</xsl:when>
			</xsl:choose>
			<xsl:choose>
				<xsl:when test="media-control/APP_SHARING_CTRL">
					<xsl:text disable-output-escaping="yes">
APP_SHARING_CTRL:</xsl:text>
					<xsl:value-of select="media-control/APP_SHARING_CTRL"/>
				</xsl:when>
			</xsl:choose>
			<xsl:choose>
				<xsl:when test="media-control/AUDIO_CTRL">
					<xsl:text disable-output-escaping="yes">
AUDIO_CTRL:</xsl:text>
					<xsl:value-of select="media-control/AUDIO_CTRL"/>
				</xsl:when>
			</xsl:choose>
			<xsl:choose>
				<xsl:when test="media-control/VIDEO_CTRL">
					<xsl:text disable-output-escaping="yes">
VIDEO_CTRL:</xsl:text>
					<xsl:value-of select="media-control/VIDEO_CTRL"/>
				</xsl:when>
			</xsl:choose>
			<xsl:choose>
				<xsl:when test="SITE_ADDITIONAL_PARAMS/ISABEL_VNC_SERVER_DISPLAY">
					<xsl:text disable-output-escaping="yes">
ADDITIONAL_PARAMS: ISABEL_VNC_SERVER_DISPLAY </xsl:text>
					<xsl:value-of select="SITE_ADDITIONAL_PARAMS/ISABEL_VNC_SERVER_DISPLAY"/>
				</xsl:when>
			</xsl:choose>
			<xsl:choose>
				<xsl:when test="SITE_ADDITIONAL_PARAMS/ISABEL_GATEWAY">
					<xsl:text disable-output-escaping="yes">
ADDITIONAL_PARAMS: ISABEL_GATEWAY </xsl:text>
					<xsl:value-of select="SITE_ADDITIONAL_PARAMS/ISABEL_GATEWAY"/>
				</xsl:when>
			</xsl:choose>
			<xsl:choose>
				<xsl:when test="SITE_ADDITIONAL_PARAMS/ISABEL_ANTENNA_ENABLED">
					<xsl:choose>
						<xsl:when test="SITE_ADDITIONAL_PARAMS/ISABEL_ANTENNA_ENABLED != 'false'">
							<xsl:text disable-output-escaping="yes">
ADDITIONAL_PARAMS: ISABEL_ANTENNA_ENABLED yes</xsl:text>
						</xsl:when>
						<xsl:otherwise>
							<xsl:text disable-output-escaping="yes">
ADDITIONAL_PARAMS: ISABEL_ANTENNA_ENABLED no</xsl:text>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
END: SITE</xsl:text>

			</xsl:if>

		</xsl:for-each>
	</xsl:template>
</xsl:stylesheet>
-->
