package isabel.gwflash;

import isabel.gw.MemberInfo;

import java.util.Timer;
import java.util.TimerTask;

public class FlashGatewayStub implements IFlashManagerListener
{
	
	
	private int ssrc = 1000;
	private FlashManager flashManager;
	
	public FlashGatewayStub()
    {		
	    flashManager = new FlashManager(new String[]{"QUESTION_REQUEST"});
		flashManager.setListener(this);
	}
	
	public void onUserConnection(FlashUser user)
	{
		System.out.println("onUserConnection: " + user.getMemberInfo().getNickName());
		MemberInfo info =  user.getMemberInfo();
		user.setMemberInfo(new MemberInfo(ssrc++, info.getNickName(),info.getAddr(), info.getFlowList()));
		final FlashUser user2 = user;
		new Timer().schedule(new TimerTask() {
			public void run() {
				flashManager.accept(user2);
			}
		}, 1000);		
	}
	
	public void onUserDisconnection(final FlashUser user)
	{
		System.out.println("onUserDisconnection: " + user.getMemberInfo().getNickName());
		new Timer().schedule(new TimerTask() {
			public void run() {
				flashManager.disconnect(user,"User disconnected.");
			}
		}, 1000);		
	}
	
	public static void main(String [ ] args)
	{
		new FlashGateway();
	}

	@Override
	public void onButtonPressed(String buttonName, FlashUser user) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onFlashClientSetParameter(FlashUser user, String name,
			Object value) {
		// TODO Auto-generated method stub
		
	}
}