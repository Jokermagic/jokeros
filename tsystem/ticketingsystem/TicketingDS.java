package ticketingsystem;

import java.util.ArrayList;
import java.util.BitSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.Random;
import java.util.concurrent.atomic.AtomicIntegerArray;
import java.util.concurrent.atomic.AtomicLong;

public class TicketingDS implements TicketingSystem {

	//ToDo
	int routenum = 5;
	int coachnum = 8;
	int seatnum = 100;
	int stationnum = 10;
	int threadnum = 16;

	AtomicLong aLong; // for tid
	Route[] routes;

	public TicketingDS(int routenum, int coachnum, int seatnum, int stationnum, int threadnum) {
		this.routenum = routenum;
		this.coachnum = coachnum;
		this.seatnum = seatnum;
		this.stationnum = stationnum;
		this.threadnum = threadnum;

		aLong = new AtomicLong();
		aLong.set(1);

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
		if (routes[route - 1].buy(ticket) == false)
			return null;
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
			for(int i=0; i<coachnum; i++) {
				tickets += coachs[i].inquiry(departure, arrival);
			}
			return tickets;
		}

		public boolean buy(Ticket ticket) {
			int randcoach = new Random().nextInt(coachnum) % coachnum;
			for (int i = randcoach; i < randcoach + coachnum; i++) {
				int coachid = i % coachnum;
				if (coachs[coachid].inquiry(ticket.departure, ticket.arrival)>0) {
					int seat = coachs[coachid].buy(ticket.departure, ticket.arrival);
					if(seat > 0) { 	// successfully bought
						ticket.coach = coachid+1;
						ticket.seat = seat;
						return true;
					}
				}
			}
			return false;
		}

		public boolean refund(Ticket ticket) {
			if (ticket.coach < 1 || ticket.coach > coachnum) return false; 
			return coachs[ticket.coach - 1].refund(ticket);
		}

	}

	/**
	 * Coach
	 */
	public class Coach {
		int seatnum;
		int stationnum;

		Seat[] seats;

		AtomicIntegerArray ticketsArr;

		public Coach(int seatnum, int stationnum) {
			this.seatnum = seatnum;
			this.stationnum = stationnum;

			seats = new Seat[seatnum];
			for (int i = 0; i < seatnum; i++) {
				seats[i] = new Seat(stationnum);
			}

			ticketsArr = new AtomicIntegerArray((stationnum - 1) * (stationnum - 1));
			for (int i = 0; i < stationnum - 1; i++) {
				for (int j = i; j < stationnum - 1; j++) {
					ticketsArr.set(i * (stationnum - 1) + j, seatnum);
				}
			}
		}

		public int inquiry(int departure, int arrival) {
			if (departure<1 || arrival>stationnum) return 0;
			int pos = (departure - 1) * (stationnum - 1) + arrival - 1 - 1;
			return ticketsArr.get(pos);
		}

		public int buy(int departure, int arrival) {
			int randseat = new Random().nextInt(seatnum) % seatnum;
			for (int i = randseat; i < randseat + seatnum; i++) {
				int seatid = i % seatnum;
				if(seats[seatid].buy(departure, arrival, ticketsArr)) 
					return seatid + 1;
			}
			return -1;
		}

		public boolean refund(Ticket ticket) {
			if (ticket.seat < 1 || ticket.seat > seatnum) {
				return false;
			}
			return seats[ticket.seat - 1].refund(ticket.departure, ticket.arrival, ticketsArr);
		}

	}

	/**
	 * Seat
	 */
	public class Seat {
		BitSet segset; // n stationnumet have n-1 segments

		public Seat(int stationnum) {
			segset = new BitSet(stationnum - 1);
			segset.clear();
		}

		synchronized public boolean buy(int departure, int arrival, AtomicIntegerArray ticketsArr) {
			BitSet bs = segset.get(departure - 1, arrival - 1);
			if (!bs.isEmpty()) return false;

			int left = 0, right = stationnum - 1;
			for (int i = departure - 1; i >= 0; i--) {
				if (segset.get(i)) {
					left = i + 1;
					break;
				}
			}
			right = segset.nextSetBit(arrival - 2);
			if (right > stationnum - 1 || right == -1) {
				right = stationnum - 1;
			}
			right--;

			for (int i = left; i < arrival - 1; i++) {
				for (int j = departure - 1; j <= right; j++) {
					if (j >= i) {
						ticketsArr.getAndDecrement(i * (stationnum - 1) + j);
					}
				}
			}
			segset.set(departure - 1, arrival - 1);
			return true;
		}

		synchronized public boolean refund(int departure, int arrival, AtomicIntegerArray ticketsArr) {
			if (departure < 1 || arrival > stationnum) {
				return false;
			}
			int left = 0, right = stationnum - 1;
			for (int i = departure - 1 - 1; i >= 0; i--) {
				if (segset.get(i)) {
					left = i + 1;
					break;
				}
			}
			right = segset.nextSetBit(arrival - 1);
			if (right > stationnum - 1 || right == -1) {
				right = stationnum - 1;
			}
			right--;

			for (int i = left; i < arrival - 1; i++) {
				for (int j = departure - 1; j <= right; j++) {
					if (j >= i) {
						ticketsArr.getAndIncrement(i * (stationnum - 1) + j);
					}
				}
			}
			segset.clear(departure - 1, arrival - 1);
			return true;
		}

	}

}
