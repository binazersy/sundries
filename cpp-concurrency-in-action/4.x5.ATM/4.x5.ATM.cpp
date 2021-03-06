/**=======================================================================

	4.x5.ATM.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include <string>
#include <iostream>
#include "messaging.hpp"


struct withdraw
{
	std::string _account;
	unsigned _amount;
	mutable messaging::sender _atm_queue;

	withdraw(std::string const& account, unsigned amount, messaging::sender atm_queue):
		_account(account), _amount(amount), _atm_queue(atm_queue)
	{}
};


struct withdraw_ok
{};


struct withdraw_denied
{};


struct cancel_withdrawal
{
	std::string _account;
	unsigned _amount;

	cancel_withdrawal(std::string const& account, unsigned amount) :
		_account(account), _amount(amount)
	{}
};



struct withdrawal_processed
{
	std::string _account;
	unsigned _amount;

	withdrawal_processed(std::string const& account, unsigned amount) :
		_account(account), _amount(amount)
	{}
};


struct card_inserted
{
	std::string _account;

	explicit card_inserted(std::string const& account):
		_account(account)
	{}
};


struct digit_pressed
{
	char _digit;

	explicit digit_pressed(char digit) :
		_digit(digit)
	{}
};


struct clear_last_pressed
{};


struct eject_card
{};


struct withdraw_pressed
{
	unsigned _amount;

	explicit withdraw_pressed(unsigned amount):
		_amount(amount)
	{}
};


struct cancel_pressed
{};


struct issue_money
{
	unsigned _amount;

	issue_money(unsigned amount) :_amount(amount) {}
};


struct verify_pin
{
	std::string _account;
	std::string _pin;
	mutable messaging::sender _atm_queue;

	verify_pin(std::string const& account, std::string const& pin, messaging::sender atm_queue)
		: _account(account), _pin(pin), _atm_queue(atm_queue)
	{}
};


struct pin_verified
{};


struct pin_incorrect
{};


struct display_enter_pin
{};


struct display_enter_card
{};


struct display_insufficient_funds
{};


struct display_withdrawal_cancelled
{};


struct display_pin_incorrect_message
{};


struct display_withdrawal_options
{};


struct get_balance
{
	std::string _account;
	mutable messaging::sender _atm_queue;

	get_balance(std::string account, messaging::sender atm_queue)
		: _account(account), _atm_queue(atm_queue)
	{}
};


struct balance
{
	unsigned _amount;

	explicit balance(unsigned amount) :_amount(amount) {}
};


struct display_balance
{
	unsigned _amount;

	explicit display_balance(unsigned amount) :_amount(amount) {}
};


struct balance_pressed
{};


class atm
{
	messaging::receiver _incoming;
	messaging::sender _bank;
	messaging::sender _hardware;

	void (atm::*state) ();

	std::string _account;
	unsigned _withdrawal_amount;
	std::string _pin;

	void process_withdrawal()
	{
		_incoming.wait().handle<withdraw_ok>(
				[&](withdraw_ok const& msg)
				{
					_hardware.send(issue_money(_withdrawal_amount));
					_bank.send(withdrawal_processed(_account, _withdrawal_amount));
					state = &atm::done_processing;
				}
		).handle<withdraw_denied>(
				[&](withdraw_denied const& msg)
				{
					_hardware.send(display_insufficient_funds());
					state = &atm::done_processing;
				}
		).handle<cancel_pressed>(
				[&](cancel_pressed const& msg)
				{
					_bank.send(cancel_withdrawal(_account, _withdrawal_amount));
					_hardware.send(display_withdrawal_cancelled());
					state = &atm::done_processing;
				}
		);
	}

	void process_balance()
	{
		_incoming.wait().handle<balance>(
				[&](balance const& msg)
				{
					_hardware.send(display_balance(msg._amount));
					state = &atm::wait_for_action;
				}
		).handle<cancel_pressed>(
				[&](cancel_pressed const& msg)
				{
					state = &atm::done_processing;
				}
		);
	}

	void wait_for_action()
	{
		_hardware.send(display_withdrawal_options());
		_incoming.wait().handle<withdraw_pressed>(
				[&](withdraw_pressed const& msg)
				{
					_withdrawal_amount = msg._amount;
					_bank.send(withdraw(_account, msg._amount, _incoming));
					state = &atm::process_withdrawal;
				}
		).handle<balance_pressed>(
				[&](balance_pressed const& msg)
				{
					_bank.send(get_balance(_account, _incoming));
					state = &atm::process_balance;
				}
		).handle<cancel_pressed>(
				[&](cancel_pressed const& msg)
				{
					state = &atm::done_processing;
				}
		);
	}

	void verifying_pin()
	{
		_incoming.wait().handle<pin_verified>(
				[&](pin_verified const& msg)
				{
					state = &atm::wait_for_action;
				}
		).handle<pin_incorrect>(
				[&](pin_incorrect const& msg)
				{
					_hardware.send(display_pin_incorrect_message());
					state = &atm::done_processing;
				}
		).handle<cancel_pressed>(
				[&](cancel_pressed const& msg)
				{
					state = &atm::done_processing;
				}
		);
	}

	void getting_pin()
	{
		_incoming.wait()
			.handle<digit_pressed>(
				[&](digit_pressed const& msg)
		{
			unsigned const pin_length = 4;
			_pin += msg._digit;
			if (_pin.length() == pin_length)
			{
				_bank.send(verify_pin(_account, _pin, _incoming));
				state = &atm::verifying_pin;
			}
		}
		).handle<clear_last_pressed>(
			[&](clear_last_pressed const& msg)
		{
			if (!_pin.empty())
			{
				_pin.pop_back();
			}
		}
		).handle<cancel_pressed>(
			[&](cancel_pressed const& msg)
		{
			state = &atm::done_processing;
		}
		);
	}

	void waiting_for_card()
	{
		_hardware.send(display_enter_card());
		_incoming.wait()
			.handle<card_inserted>(
				[&](card_inserted const& msg)
		{
			_account = msg._account;
			_pin = "";
			_hardware.send(display_enter_pin());
			state = &atm::getting_pin;
		}
		);
	}

	void done_processing()
	{
		_hardware.send(eject_card());
		state = &atm::waiting_for_card;
	}

	atm(atm const&) = delete;
	atm& operator=(atm const&) = delete;

public:
	atm(messaging::sender bank, messaging::sender hardware)
		: _bank(bank), _hardware(hardware)
	{}

	void done()
	{
		get_sender().send(messaging::close_queue());
	}

	void run()
	{
		state = &atm::waiting_for_card;
		try
		{
			for (;;)
				(this->*state) ();
		}
		catch (messaging::close_queue const&)
		{
		}
	}

	messaging::sender get_sender()
	{
		return _incoming;
	}
};


class bank_machine
{
	messaging::receiver _incoming;
	unsigned _balance;

public:
	bank_machine() : _balance(199) {}

	void done()
	{
		get_sender().send(messaging::close_queue());
	}

	void run()
	{
		try
		{
			for (;;)
			{
				_incoming.wait().handle<verify_pin>(
					[&](verify_pin const& msg)
				{
					if (msg._pin == "1937")
					{
						msg._atm_queue.send(pin_verified());
					}
					else
					{
						msg._atm_queue.send(pin_incorrect());
					}
				}
				).handle<withdraw>(
					[&](withdraw const& msg)
				{
					if (_balance >= msg._amount)
					{
						msg._atm_queue.send(withdraw_ok());
						_balance -= msg._amount;
					}
					else
					{
						msg._atm_queue.send(withdraw_denied());
					}
				}
				).handle<get_balance>(
					[&](get_balance const& msg)
				{
					msg._atm_queue.send(::balance(_balance));
				}
				).handle<withdrawal_processed>(
					[&](withdrawal_processed const& msg)
				{
				}
				).handle<cancel_withdrawal>(
					[&](cancel_withdrawal const& msg)
				{
				}
				);

			}
		}
		catch (messaging::close_queue const&)
		{
		}
	}

	messaging::sender get_sender()
	{
		return _incoming;
	}
};


class interface_machine
{
	messaging::receiver _incoming;
	std::mutex iom;

public:
	void done()
	{
		get_sender().send(messaging::close_queue());
	}

	void run()
	{
		try
		{
			for (;;)
			{
				_incoming.wait().handle<issue_money>(
					[&](issue_money const& msg)
				{
					{
						std::lock_guard<std::mutex> lock(iom);
						std::cout << "Issuing "
							<< msg._amount << std::endl;
					}
				}
				).handle<display_insufficient_funds>(
					[&](display_insufficient_funds const& msg)
				{
					{
						std::lock_guard<std::mutex> lk(iom);
						std::cout << "Insufficient funds" << std::endl;
					}
				}
				).handle<display_enter_pin>(
					[&](display_enter_pin const& msg)
				{
					{
						std::lock_guard<std::mutex> lk(iom);
						std::cout
							<< "Please enter your PIN (0-9)"
							<< std::endl;
					}
				}
				).handle<display_enter_card>(
					[&](display_enter_card const& msg)
				{
					{
						std::lock_guard<std::mutex> lk(iom);
						std::cout << "Please enter your card (I)"
							<< std::endl;
					}
				}
				).handle<display_balance>(
					[&](display_balance const& msg)
				{
					{
						std::lock_guard<std::mutex> lk(iom);
						std::cout
							<< "The balance of your account is "
							<< msg._amount << std::endl;
					}
				}
				).handle<display_withdrawal_options>(
					[&](display_withdrawal_options const& msg)
				{
					{
						std::lock_guard<std::mutex> lk(iom);
						std::cout << "Withdraw 50? (w)" << std::endl;
						std::cout << "Display Balance? (b)"
							<< std::endl;
						std::cout << "Cancel? (c)" << std::endl;
					}
				}
				).handle<display_withdrawal_cancelled>(
					[&](display_withdrawal_cancelled const& msg)
				{
					{
						std::lock_guard<std::mutex> lk(iom);
						std::cout << "Withdrawal cancelled"
							<< std::endl;
					}
				}
				).handle<display_pin_incorrect_message>(
					[&](display_pin_incorrect_message const& msg)
				{
					{
						std::lock_guard<std::mutex> lk(iom);
						std::cout << "PIN incorrect" << std::endl;
					}
				}
				).handle<eject_card>(
					[&](eject_card const& msg)
				{
					{
						std::lock_guard<std::mutex> lk(iom);
						std::cout << "Ejecting card" << std::endl;
					}
				}
				);
			}
		}
		catch (messaging::close_queue&)
		{
		}
	}

	messaging::sender get_sender()
	{
		return _incoming;
	}
};


int main()
{
	bank_machine bank;
	interface_machine interface_hardware;
	atm machine(bank.get_sender(), interface_hardware.get_sender());
	std::thread bank_thread(&bank_machine::run, &bank);
	std::thread if_thread(&interface_machine::run, &interface_hardware);
	std::thread atm_thread(&atm::run, &machine);
	messaging::sender atmqueue(machine.get_sender());
	bool quit_pressed = false;
	while (!quit_pressed)
	{
		char c = getchar();
		switch (c)
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			atmqueue.send(digit_pressed(c));
			break;
		case 'b':
			atmqueue.send(balance_pressed());
			break;
		case 'w':
			atmqueue.send(withdraw_pressed(50));
			break;
		case 'c':
			atmqueue.send(cancel_pressed());
			break;
		case 'q':
			quit_pressed = true;
			break;
		case 'i':
			atmqueue.send(card_inserted("acc1234"));
			break;
		}
	}

	bank.done();
	machine.done();
	interface_hardware.done();
	atm_thread.join();
	bank_thread.join();
	if_thread.join();

	return 0;
}

