package ticketingsystem;

import java.util.AbstractMap;
import java.util.BitSet;
import java.util.HashMap;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Random;
import java.util.AbstractMap.SimpleEntry;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentLinkedDeque;
import java.util.concurrent.atomic.AtomicIntegerArray;
import java.util.concurrent.atomic.AtomicLong;

public class TicketingDS implements TicketingSystem {

	//ToDo
	int routenum = 5;
	int coachnum = 8;
	int seatnum = 100;
	int stationnum = 10;
	int threadnum = 16;

	AtomicLong aLong = new AtomicLong(1);; // for tid
	Route[] routes;

	public TicketingDS(int routenum, int coachnum, int seatnum, int stationnum, int threadnum) {
		this.routenum = routenum;
		this.coachnum = coachnum;
		this.seatnum = seatnum;
		this.stationnum = stationnum;
		this.threadnum = threadnum;


		this.routes = new Route[routenum];
		for (int i = 0; i < routenum; i++) {
			routes[i] = new Route(coachnum, seatnum, stationnum);
		}
	}

	@Override
	public Ticket buyTicket(String passenger, int route, int departure, int arrival) {
		Ticket ticket = new Ticket();
		ticket.tid = aLong.getAndIncrement();
		ticket.passenger = passenger;
		ticket.route = route;
		ticket.coach = 0;
		ticket.seat = 0;
		ticket.departure = departure;
		ticket.arrival = arrival;
		if (routes[route - 1].buy(ticket) == false) {
			return null;
		}
		return ticket;
	}

	@Override
	public int inquiry(int route, int departure, int arrival) {
		return routes[route - 1].inquiry(departure, arrival);
	}

	@Override
	public boolean refundTicket(Ticket ticket) {
		return routes[ticket.route - 1].refund(ticket);
	}

	/**
	 * Route
	 */
	public class Route {

		int coachnum;
		int seatnum;
		int stationnum;

		Coach[] coachs;

		public Route(int coachnum, int seatnum, int stationnum) {
			this.coachnum = coachnum;
			this.seatnum = seatnum;
			this.stationnum = stationnum;

			coachs = new Coach[coachnum];
			for (int i = 0; i < coachnum; i++) {
				coachs[i] = new Coach(seatnum, stationnum);
			}

		}

		public int inquiry(int departure, int arrival) {
			int tickets = 0;
			for (int i = 0; i < coachnum; i++) {
				tickets += coachs[i].inquiry(departure, arrival);
			}
			return tickets;
		}


		public boolean buy(Ticket ticket) {
			int randcoach = new Random().nextInt(coachnum) % coachnum;
			for (int i = randcoach; i < randcoach + coachnum; i++) {
				int coachid = i % coachnum;
				if (coachs[coachid].inquiry(ticket.departure, ticket.arrival) > 0) {
					int seat = coachs[coachid].buy(ticket.departure, ticket.arrival);
					if (seat > 0) { // successfully bought
						ticket.coach = coachid + 1;
						ticket.seat = seat;
						return true;
					}
				}
			}
			return false;
		}

		public boolean refund(Ticket ticket) {
			if (ticket.coach < 1 || ticket.coach > coachnum)
				return false;
			return coachs[ticket.coach - 1].refund(ticket);
		}

	}

	/**
	 * Coach
	 */
	public class Coach {
		int seatnum;
		int stationnum;

		HashMap<String, ConcurrentLinkedDeque> hashMap;

		public Coach(int seatnum, int stationnum) {

			this.seatnum = seatnum;
			this.stationnum = stationnum;

			hashMap = new HashMap<String, ConcurrentLinkedDeque>();
			for (int i = 1; i < stationnum; i++) {
				for (int j = i + 1; j <= stationnum; j++) {
					hashMap.put(i + "_" + j, new ConcurrentLinkedDeque<Integer>());
				}
			}

			ConcurrentLinkedDeque deque = hashMap.get(1 + "_" + stationnum);
			for (int k = 1; k <= seatnum; k++) {
				deque.add(k);
			}

		}

		public int inquiry(int departure, int arrival) {
			if (departure < 1 || arrival > stationnum)
				return 0;

			int sum = 0;
			for (int left = 1; left <= departure; left++) {
				for (int right = arrival; right <= stationnum; right++) {
					sum += hashMap.get(left + "_" + right).size();
				}
			}
			return sum;
		}

		public int buy(int departure, int arrival) {
			if (departure < 1 || arrival > stationnum)
				return -1;

			ConcurrentLinkedDeque deque;
			for (int left = departure; left >= 1; left--) {
				for (int right = arrival; right <= stationnum; right++) {
					deque = hashMap.get(left + "_" + right);
					if (deque.size() > 0) {
						try {
							int seat = (int)deque.pop();
							if (left < departure)
								hashMap.get(left + "_" + departure).push(seat);
							if (arrival < right)
								hashMap.get(arrival + "_" + right).push(seat);
							return seat;
						} catch (NoSuchElementException e) {
							//TODO: handle exception
						}

					}
				}
			}
			return -1;
		}

		public boolean refund(Ticket ticket) {
			if (ticket.seat < 1 || ticket.seat > seatnum) return false;

			ConcurrentLinkedDeque leftqueue = null, rightqueue = null;
			int left, right;

			for(left = ticket.departure-1; left>=1; left--) {
				leftqueue = hashMap.get(left+"_"+ticket.departure);
				if (leftqueue.contains(ticket.seat)) {
					leftqueue.remove(ticket.seat);
					break;
				}
			}
			if(left<1) left = 1;
			for(right = ticket.arrival+1; right<=stationnum; right++) {
				rightqueue = hashMap.get(ticket.arrival+"_"+right);
				if (rightqueue.contains(ticket.seat)) {
					rightqueue.remove(ticket.seat);
					break;
				}
			}
			if(right>stationnum) right = stationnum;
			
			hashMap.get(left+"_"+right).remove(ticket.seat);
			return true;
		}

	}
}
