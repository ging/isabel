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
	<xsl:template match="/">
		<xsl:text disable-output-escaping="yes">BEGIN: EDL
VERSION: </xsl:text>
		<xsl:value-of select="EDL/VERSION"/>
		<xsl:text disable-output-escaping="yes">
BEGIN: SESSION</xsl:text>
		<xsl:for-each select="EDL/SESSION">
			<xsl:text disable-output-escaping="yes">
ID:</xsl:text>
			<xsl:value-of select="ID"/>
			<xsl:text disable-output-escaping="yes">
DELIVERY_PLATFORM:</xsl:text>
			<xsl:value-of select="DELIVERY_PLATFORM"/>
			<xsl:choose>
				<xsl:when test="SESSION_ADDITIONAL_PARAMS">
					<xsl:choose>
						<xsl:when test="SESSION_ADDITIONAL_PARAMS/ISABEL_ENABLE_VNC_MODE">
							<xsl:choose>
								<xsl:when test="SESSION_ADDITIONAL_PARAMS/ISABEL_ENABLE_VNC_MODE != 'false'">
									<xsl:text disable-output-escaping="yes">
ADDITIONAL_PARAMS: ISABEL_ENABLE_VNC_MODE yes</xsl:text>
								</xsl:when>
								<xsl:otherwise>
									<xsl:text disable-output-escaping="yes">
ADDITIONAL_PARAMS: ISABEL_ENABLE_VNC_MODE no</xsl:text>
								</xsl:otherwise>
							</xsl:choose>
						</xsl:when>
					</xsl:choose>
					<xsl:choose>
						<xsl:when test="SESSION_ADDITIONAL_PARAMS/ISABEL_ENABLE_VMware_MODE">
							<xsl:choose>
								<xsl:when test="SESSION_ADDITIONAL_PARAMS/ISABEL_ENABLE_VMware_MODE != 'false'">
									<xsl:text disable-output-escaping="yes">
ADDITIONAL_PARAMS: ISABEL_ENABLE_VMware_MODE yes</xsl:text>
								</xsl:when>
								<xsl:otherwise>
									<xsl:text disable-output-escaping="yes">
ADDITIONAL_PARAMS: ISABEL_ENABLE_VMware_MODE no</xsl:text>
								</xsl:otherwise>
							</xsl:choose>
						</xsl:when>
					</xsl:choose>
					<xsl:choose>
						<xsl:when test="SESSION_ADDITIONAL_PARAMS/ISABEL_DEFAULT_VNC_SERVER_DISPLAY">
							<xsl:text disable-output-escaping="yes">
ADDITIONAL_PARAMS: ISABEL_DEFAULT_VNC_SERVER_DISPLAY </xsl:text>
							<xsl:value-of select="SESSION_ADDITIONAL_PARAMS/ISABEL_DEFAULT_VNC_SERVER_DISPLAY"/>
						</xsl:when>
					</xsl:choose>
					<xsl:choose>
						<xsl:when test="SESSION_ADDITIONAL_PARAMS/ISABEL_ISABEL_AUDIO_BYPASS_1">
							<xsl:choose>
								<xsl:when test="SESSION_ADDITIONAL_PARAMS/ISABEL_ISABEL_AUDIO_BYPASS_1 != 'false'">
									<xsl:text disable-output-escaping="yes">
ADDITIONAL_PARAMS: ISABEL_ISABEL_AUDIO_BYPASS_1</xsl:text>
								</xsl:when>
							</xsl:choose>
						</xsl:when>
					</xsl:choose>
					<xsl:choose>
						<xsl:when test="SESSION_ADDITIONAL_PARAMS/ISABEL_ISABEL_AUDIO_BYPASS_2">
							<xsl:choose>
								<xsl:when test="SESSION_ADDITIONAL_PARAMS/ISABEL_ISABEL_AUDIO_BYPASS_2 != 'false'">
									<xsl:text disable-output-escaping="yes">
ADDITIONAL_PARAMS: ISABEL_ISABEL_AUDIO_BYPASS_2</xsl:text>
								</xsl:when>
							</xsl:choose>
						</xsl:when>
					</xsl:choose>
					<xsl:choose>
						<xsl:when test="SESSION_ADDITIONAL_PARAMS/ISABEL_ISABEL_AUDIO_BYPASS_3">
							<xsl:choose>
								<xsl:when test="SESSION_ADDITIONAL_PARAMS/ISABEL_ISABEL_AUDIO_BYPASS_3 != 'false'">
									<xsl:text disable-output-escaping="yes">
ADDITIONAL_PARAMS: ISABEL_ISABEL_AUDIO_BYPASS_3</xsl:text>
								</xsl:when>
							</xsl:choose>
						</xsl:when>
					</xsl:choose>
					<xsl:choose>
						<xsl:when test="SESSION_ADDITIONAL_PARAMS/IPSEC_ENABLE">
							<xsl:choose>
								<xsl:when test="SESSION_ADDITIONAL_PARAMS/IPSEC_ENABLE != 'false'">
									<xsl:text disable-output-escaping="yes">
ADDITIONAL_PARAMS: IPSEC_ENABLE yes</xsl:text>
								</xsl:when>
								<xsl:otherwise>
									<xsl:text disable-output-escaping="yes">
ADDITIONAL_PARAMS: IPSEC_ENABLE no</xsl:text>
								</xsl:otherwise>
							</xsl:choose>
						</xsl:when>
					</xsl:choose>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
SERVICE: </xsl:text>
			<xsl:choose>
				<xsl:when test="SERVICE/SERVICE-NAME = 'telemeeting'">
					<xsl:text disable-output-escaping="yes">meeting.act</xsl:text>
				</xsl:when>
				<xsl:when test="SERVICE/SERVICE-NAME = 'teleclass'">
					<xsl:text disable-output-escaping="yes">class.act</xsl:text>
				</xsl:when>
				<xsl:when test="SERVICE/SERVICE-NAME = 'teleconference'">
					<xsl:text disable-output-escaping="yes">conference.act</xsl:text>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="SERVICE/SERVICE-NAME"/>
				</xsl:otherwise>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
QUALITY: </xsl:text>
			<xsl:value-of select="SERVICE/SERVICE-QUALITY"/>
			<xsl:text disable-output-escaping="yes">
SESSION_PORT: </xsl:text>
			<xsl:value-of select="session-info/SESSION_PORT"/>
			<xsl:choose>
				<xsl:when test="session-info/MASTER">
					<xsl:text disable-output-escaping="yes">
MASTER: </xsl:text>
					<xsl:value-of select="session-info/MASTER"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:text disable-output-escaping="yes">
MASTER: </xsl:text>
					<xsl:text>127.0.0.1</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
			<xsl:choose>
				<xsl:when test="session-info/OPEN_SESSION">
					<xsl:text disable-output-escaping="yes">
OPEN_SESSION: </xsl:text>
					<xsl:choose>
						<xsl:when test="session-info/OPEN_SESSION != 'false'">
							<xsl:text disable-output-escaping="yes">yes</xsl:text>
						</xsl:when>
						<xsl:otherwise>
							<xsl:text disable-output-escaping="yes">no</xsl:text>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:when>
			</xsl:choose>
			<xsl:choose>
				<xsl:when test="session-info/ALLOW_WATCHPOINTS">
					<xsl:text disable-output-escaping="yes">
ALLOW_WATCHPOINTS:</xsl:text>
					<xsl:choose>
						<xsl:when test="session-info/ALLOW_WATCHPOINTS != 'false'">
							<xsl:text disable-output-escaping="yes">yes</xsl:text>
						</xsl:when>
						<xsl:otherwise>
							<xsl:text disable-output-escaping="yes">no</xsl:text>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:when>
			</xsl:choose>
			<xsl:text disable-output-escaping="yes">
MAIL_ADDRESS:</xsl:text>
			<xsl:value-of select="session-info/MAIL_ADDRESS"/>
			<xsl:text disable-output-escaping="yes">
DESCRIPTION:</xsl:text>
			<xsl:value-of select="session-info/DESCRIPTION"/>
			<xsl:choose>
				<xsl:when test="session-info/SITE-IDS">
					<xsl:text disable-output-escaping="yes">
SITES: </xsl:text>
					<xsl:value-of select="session-info/SITE-IDS"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:text disable-output-escaping="yes">
SITES: </xsl:text>
					<xsl:for-each select="/EDL/site-description/SITE">
						<xsl:value-of select="site-identification/ID"/>
						<xsl:text> </xsl:text>
					</xsl:for-each>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>
		<xsl:text disable-output-escaping="yes">
END: SESSION</xsl:text>
		<xsl:for-each select="EDL/site-description/SITE">
			<xsl:text disable-output-escaping="yes">

BEGIN: SITE</xsl:text>
			<xsl:text disable-output-escaping="yes">
ID:</xsl:text>
			<xsl:value-of select="site-identification/ID"/>
			<xsl:choose>
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
		</xsl:for-each>
		<xsl:text disable-output-escaping="yes">

END: EDL
</xsl:text>
	</xsl:template>
</xsl:stylesheet>
