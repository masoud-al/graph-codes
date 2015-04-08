/* 
 * File:   channel.h
 * Author: masoud
 *
 * Created on February 13, 2012, 7:42 PM
 */

#ifndef CHANNEL_H
#define	CHANNEL_H

#include <vector>

typedef  double  LLRType;

//#define mylog logf
//#define myexp expf
#define mylog log
#define myexp exp


class Channel{
public:
	double param;
	void *ch;
	Channel(double p);
	inline unsigned int roundPseudoCodeword(const LLRType* solution, unsigned int length){
		unsigned int c=0;
		for(unsigned int i=0;i<length;i++)
			c+=solution[i]>0.5;
		return c;
	}
	inline void roundPseudoCodeword(const LLRType* solution, bool* rounded, unsigned int length){
		for(unsigned int i=0;i<length;i++){
			rounded[i]=solution[i]>0.5;
		}
	}

	virtual double capacity(double)=0;
	virtual double capacity()=0;
	virtual double setCapacity(double)=0;
	virtual unsigned int transmitzero(std::vector<LLRType>& llr)=0;  
	virtual unsigned int transmitzero(LLRType* llr, unsigned int length)=0;  
	//virtual void decode(const std::vector<LLRType>& llr,std::vector<bool>)=0;
	virtual void print()=0;
};

class BSC: public Channel{
public:    
	BSC(double );
	~BSC();
	double capacity(double);
	double capacity(){return capacity(param);}
	unsigned int transmitzero(std::vector<LLRType>& llr);
	unsigned int transmitzero(LLRType* llr, unsigned int length);
	//void decode(const std::vector<LLRType>& llr,std::vector<bool>);
	double setCapacity(double);
	void print();
};

//parameter is SNR in dB
class AWGN: public Channel{
private:
	double variance;
	
public:    
	AWGN(double );  
	~AWGN();
	double capacity(){return capacity(param);}
	double capacity(double);
	unsigned int transmitzero(std::vector<LLRType>& llr);
	unsigned int transmitzero(LLRType* llr, unsigned int N);
	//void decode(const std::vector<LLRType>& llr,std::vector<bool>);
	double setCapacity(double);
	static inline double SNRtoVariance(double p){
		return std::sqrt(std::pow(10,-p/10));
	}
	void print();

};



#endif	/* CHANNEL_H */
