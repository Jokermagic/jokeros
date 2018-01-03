package ticketingsystem;

import java.util.ArrayList;
import java.util.Random;

public class Test {
	final static int threadnum = 4; // concurrent thread number
	final static int routenum = 10; // route is designed from 1 to 3
	final static int coachnum = 8; // coach is arranged from 1 to 5
	final static int seatnum = 100; // seat is allocated from 1 to 20
	final static int stationnum = 10; // station is designed from 1 to 5

	final static int testnum = 1000000;
	final static int retpc = 10; // return ticket operation is 10% percent
	final static int buypc = 40; // buy ticket operation is 30% percent
	final static int inqpc = 100; //inquiry ticket operation is 60% percent

	static String passengerName() {
		Random rand = new Random();
		long uid = rand.nextInt(testnum);
		return "passenger" + uid; 
	}

	public static void main(String[] args) throws InterruptedException {
		
		Thread[] threads = new Thread[threadnum];		

		final TicketingDS tds = new TicketingDS(routenum, coachnum, seatnum, stationnum, threadnum);
		//TODO
		long begintime = System.currentTimeMillis();
		
		final Log refundLog = new Log();
		final Log buyLog = new Log();
		final Log inquiryLog = new Log();

		for (int i = 0; i< threadnum; i++) {
	    	threads[i] = new Thread(new Runnable() {
                public void run() {
            		Random rand = new Random();
                	Ticket ticket = new Ticket();
            		ArrayList<Ticket> soldTicket = new ArrayList<Ticket>();
            		
            		for (int i = 0; i < testnum; i++) {
						// System.out.println(ThreadId.get() + " i="+i);
            			int sel = rand.nextInt(inqpc);
						if (0 <= sel && sel < retpc && soldTicket.size() > 0) { // return ticket
            				int select = rand.nextInt(soldTicket.size());
							long begin = System.currentTimeMillis();
							if ((ticket = soldTicket.remove(select)) != null) {
            					if (tds.refundTicket(ticket)) {
            						// System.out.println("TicketRefund" + " " + ticket.tid + " " + ticket.passenger + " " + ticket.route + " " + ticket.coach  + " " + ticket.departure + " " + ticket.arrival + " " + ticket.seat);
            						// System.out.flush();
            					} else {
            						// System.out.println("ErrOfRefund");
            						// System.out.flush();
            					}
            				} else {
								// System.out.println("ErrOfRefund");
        						// System.out.flush();
							}
							long end = System.currentTimeMillis();
							refundLog.count ++;
							refundLog.time += (int)(end-begin);
            			} else if (retpc <= sel && sel < buypc) { // buy ticket
            				String passenger = passengerName();
            				int route = rand.nextInt(routenum) + 1;
            				int departure = rand.nextInt(stationnum - 1) + 1;
            				int arrival = departure + rand.nextInt(stationnum - departure) + 1; // arrival is always greater than departure
							long begin = System.currentTimeMillis();
							if ((ticket = tds.buyTicket(passenger, route, departure, arrival)) != null) {
            					soldTicket.add(ticket);
            					// System.out.println("TicketBought" + " " + ticket.tid + " " + ticket.passenger + " " + ticket.route + " " + ticket.coach + " " + ticket.departure + " " + ticket.arrival + " " + ticket.seat);
        						// System.out.flush();
            				} else {
            					// System.out.println("TicketSoldOut" + " " + route+ " " + departure+ " " + arrival);
        						// System.out.flush();
							}
							long end = System.currentTimeMillis();
							buyLog.count ++;
							buyLog.time += (int)(end-begin);
            			} else if (buypc <= sel && sel < inqpc) { // inquiry ticket
            				
            				int route = rand.nextInt(routenum) + 1;
            				int departure = rand.nextInt(stationnum - 1) + 1;
            				int arrival = departure + rand.nextInt(stationnum - departure) + 1; // arrival is always greater than departure
							long begin = System.currentTimeMillis();
							int leftTicket = tds.inquiry(route, departure, arrival);
            				// System.out.println("RemainTicket" + " " + leftTicket + " " + route+ " " + departure+ " " + arrival);
							// System.out.flush();  
							long end = System.currentTimeMillis();
							inquiryLog.count ++;
							inquiryLog.time += (int)(end-begin);
    						         			
            			}
            		}

                }
            });
              threads[i].start();
 	    }
	
	    for (int i = 0; i< threadnum; i++) {
	    	threads[i].join();
		}
		long endtime = System.currentTimeMillis();
		int time = (int)(endtime-begintime);
		int concurrency = threadnum*testnum/time;
		System.out.println("finished endtime-begintime="+time);		
		System.out.println("threadnum*testnum/time="+concurrency);
		System.out.println("refundtime+count="+refundLog.time+ " + "+refundLog.count);
		System.out.println("buytime+count="+buyLog.time+ " + "+buyLog.count);
		System.out.println("inquiry time+count="+inquiryLog.time+ " + "+inquiryLog.count);
	}
	



}