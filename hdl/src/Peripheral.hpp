#ifndef PERIPHERAL_HPP
#define PERIPHERAL_HPP

// Virtual base class for Verilator peripherals

class Peripheral
{
public:
	Peripheral() {};
	~Peripheral() {};

	// Evaluate inputs to Peripheral, but don't update outputs
	virtual void eval_in() = 0;

	// Update outputs from peripheral, but don't evaluate inputs
	virtual void eval_out() = 0;
};

#endif
