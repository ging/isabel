package isabel.gwflash;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import isabel.gw.SessionEvent;
import isabel.gw.SessionEvent.Person;
import isabel.gwflash.red5client.GWFlashClient;

public class IsabelEventManager {

	enum Types {
		SESSION(0, "session"),
		PARTICIPATION(1, "participation"),
		QUESTION(2, "questions_mode"),
		SLIDE(2, "slides"),
		OTHER(2, "other");

		private int level;
		private String typeName;
		
		private Types(int level, String typeName) {
			this.level = level;
			this.typeName = typeName;
		}
		
		public int getLevel() {
			return level;
		}
		public String getTypeName() {
			return typeName;
		}
	}
	
	private String url;
	private String conferenceId;
	
	private GWFlashClient client;
	private boolean inSession = false;
	private boolean inParticipation = false;
	private boolean inQuestions = false;
	private boolean inSlides = false;
		
	private String currentSession = "";
	private String currentParticipation = "";
	
	private int eventId = 0;
	
	public IsabelEventManager() {		
		url = FlashGatewayEnvConfig.getFlashMetadataURL();
		conferenceId = FlashGatewayEnvConfig.getFlashSessionID();
		if (url != null) {
			try {
				client = new GWFlashClient(url, new String[]{"eventGenerator","acitamelet"});
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}
	
	public void isabelEvent(SessionEvent evt) {
		if (client != null) {
		System.out.println("onIsabelSessionEvent " + evt);
			Map<String, String> who = new HashMap<String, String>();
			for (Person person: evt.getPeople()) {
				if (person.getName().equals(""))
					who.put(person.getSite(), "");
				else
				    who.put(person.getName(), person.getPosition() + " " + person.getOrganization());
			}
			
//			System.out.println("onIsabelSessionEvent WHO " + who);			
//			System.out.println("onIsabelSessionEvent SESSION " + evt.getSession() + ":" + currentSession);
//			System.out.println("onIsabelSessionEvent PARTICIPATION " + evt.getParticipation() + ":" + currentParticipation);
//			System.out.println("onIsabelSessionEvent MODE " + evt.getMode());
			
			if (isIdle(evt.getMode())) {
				sendCloseEvents(Types.SESSION, evt.getDate());
				currentParticipation = "";
				currentSession = "";
			}
			else {
				if (isNewSession(evt.getSession())) {
					sendCloseEvents(Types.SESSION, evt.getDate());
					sendSessionInit(evt.getDate(), evt.getSession());
				}
				
				if (isNewParticipation(evt.getParticipation())) {
					sendCloseEvents(Types.PARTICIPATION, evt.getDate());
					sendParticipationInit(evt.getDate(), evt.getParticipation(), who);
				}
				
				sendCloseEvents(Types.OTHER, evt.getDate());
				
				if (isQuestions(evt.getMode())) {
					sendQuestionsInit(evt.getDate());
				}			
				if (isSlides(evt.getMode())) {
					sendSlidesInit(evt.getDate(), evt.getParticipation());
				}
			}
		}
	}
	
	private boolean isNewParticipation(String participation) {
		return ((participation.equals("") && currentParticipation.equals("")) || !participation.equalsIgnoreCase(currentParticipation));
	}

	private boolean isNewSession(String session) {
		return !session.equalsIgnoreCase(currentSession);
	}
	
	private boolean isIdle(String mode) {
		return mode.equalsIgnoreCase(SessionEvent.IDLE_MODE);
	}
	
	private boolean isSlides(String mode) {
		return mode.equalsIgnoreCase(SessionEvent.APP_SHARINNG_MODE);
	}

	private boolean isQuestions(String mode) {
		return mode.equalsIgnoreCase(SessionEvent.QUESTIONS_MODE);		
	}

	private void sendSlidesInit(Date date, String participation) {
		call(date, Types.SLIDE.getLevel(), Types.SLIDE.getTypeName() + "_init", participation, null);
		inSlides = true;
	}

	private void sendQuestionsInit(Date date) {
		call(date, Types.QUESTION.getLevel(), Types.QUESTION.getTypeName() + "_init", null, null);
		inQuestions = true;
	}

	private void sendParticipationInit(Date date, String participation,
			Map<String, String> who) {
		if(!inSession){
			sendSessionInit(date, "");
		}
		call(date, Types.PARTICIPATION.getLevel(), Types.PARTICIPATION.getTypeName() + "_init", participation, who);
		inParticipation = true;		
		currentParticipation = participation;
	}

	private void sendSessionInit(Date date, String conference) {
		call(date, Types.SESSION.getLevel(), Types.SESSION.getTypeName() + "_init", conference, null);
		inSession = true;
		currentSession = conference;
	}

	private void sendCloseEvents(Types level, Date date) {
		switch(level) {
			case SESSION:
				if(inSlides)
					sendEnd(date, Types.SLIDE);
				if(inQuestions)
					sendEnd(date, Types.QUESTION);
				if(inParticipation)
					sendEnd(date, Types.PARTICIPATION);
				if(inSession)
				    sendEnd(date, Types.SESSION);
				break;
			case PARTICIPATION:
				if(inSlides)
					sendEnd(date, Types.SLIDE);
				if(inQuestions)
					sendEnd(date, Types.QUESTION);
				if(inParticipation)
					sendEnd(date, Types.PARTICIPATION);
				break;
				
			case OTHER:
				if (inSlides)
					sendEnd(date, Types.SLIDE);
				if (inQuestions)
					sendEnd(date, Types.QUESTION);
				break;
		}
	}

	private void sendEnd(Date date, Types type) {
		call(date, type.getLevel(), type.getTypeName()+"_end", null, null);
		switch(type) {
		case SESSION:
			inSession = false;
			break;
		case PARTICIPATION:
			inParticipation = false;
			break;
		case QUESTION:
			inQuestions = false;
			break;
		case SLIDE:
			inSlides = false;
			break;
		}
	}
	
	private void call(Date date, int level, String type, String title, Map<String, String> who) {
		Object[] params = {conferenceId, ++eventId, date, level, type, title, who}; 
		client.getNetConnection().call("onIsabelSessionEvent", params);
		System.out.println("onIsabelSessionEvent(" + conferenceId + ", "+ ++eventId + ", " + date + ", " + level + ", " + type + ", " + who + ")");
	}
}
