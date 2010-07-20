#ifndef ITNORM_H_
#define ITNORM_H_

#define ITERINHI 2.0f
#define ITERCOEX 0.5f
#define ITERCOIN 1.5f
#define ITEREXSIG 2.0f
#define ITERINSIG 25.0f

class ItNorm
{
public:
	ItNorm(int w, int h);
	virtual ~ItNorm();
	
	void 		iterativeNormalize(float* img, float* dst, int numIter, float min=0, float max=0);
	
protected:
	void 		convoluteX(float* kernel, int ksize, float ksum, float* src, float* dst);
	void 		convoluteY(float* kernel, int ksize, float ksum, float* src, float* dst);

	float* 		gaussian(float peak, float sigma, int maxhw, int* datasize);
	void 		createITKernel();
	void 		clampZero(float* img, float* dst);
	void 		calcMinMax(float* src, float* min, float* max);
	void 		calcDiff(float* src, float* dst, float* imgInh, float* imgExc);
	void 		normalizeImg(float* img, float rangeMin, float rangeMax);
	
	
protected:
	int			m_height;
	int			m_width;
	float* 		m_gExc;
	float* 		m_gInh;
	float		m_gExcSum;
	float		m_gInhSum;
	int			m_gExcSize;
	int			m_gInhSize;
	
	float* 		m_tmp1;
	float* 		m_imgExc;
	float* 		m_imgInh;
	
	
};

#endif /*ITNORM_H_*/
