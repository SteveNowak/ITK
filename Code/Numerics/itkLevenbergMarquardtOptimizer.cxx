/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkLevenbergMarquardtOptimizer.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkLevenbergMarquardtOptimizer_txx
#define _itkLevenbergMarquardtOptimizer_txx

#include "itkLevenbergMarquardtOptimizer.h"

namespace itk
{

/**
 * Constructor
 */
LevenbergMarquardtOptimizer
::LevenbergMarquardtOptimizer()
{
  m_OptimizerInitialized    = false;
  m_VnlOptimizer            = 0;
  m_NumberOfIterations      = 2000;
  m_ValueTolerance          = 1e-8;
  m_GradientTolerance       = 1e-5;
  m_EpsilonFunction         = 1e-11;
}


/**
 * Destructor
 */
LevenbergMarquardtOptimizer
::~LevenbergMarquardtOptimizer()
{
  delete m_VnlOptimizer;
}



/**
 * Connect a Cost Function
 */
void
LevenbergMarquardtOptimizer
::SetCostFunction( MultipleValuedCostFunction * costFunction )
{
  const unsigned int numberOfParameters = 
    costFunction->GetNumberOfParameters();

  const unsigned int numberOfValues = 
    costFunction->GetNumberOfValues();

  CostFunctionAdaptorType * adaptor = 
    new CostFunctionAdaptorType( numberOfParameters, numberOfValues );
       
  adaptor->SetCostFunction( costFunction );

  if( m_OptimizerInitialized )
    { 
    delete m_VnlOptimizer;
    }
    
  this->SetCostFunctionAdaptor( adaptor );

  m_VnlOptimizer = new vnl_levenberg_marquardt( *adaptor );

  ScalesType scales( numberOfParameters );
  scales.Fill( 1.0f );
  SetScales( scales );

  this->SetNumberOfIterations(m_NumberOfIterations);
  this->SetValueTolerance(m_ValueTolerance);
  this->SetGradientTolerance(m_GradientTolerance);
  this->SetEpsilonFunction(m_EpsilonFunction);

  m_OptimizerInitialized = true;

}



/**
 * Start the optimization
 */
void
LevenbergMarquardtOptimizer
::StartOptimization( void )
{
  
  ParametersType initialPosition = GetInitialPosition();

  InternalParametersType parameters( initialPosition.Size() );

  CostFunctionAdaptorType::ConvertExternalToInternalParameters( 
    GetInitialPosition(), 
    parameters     );

  if( this->GetCostFunctionAdaptor()->GetUseGradient() )
    {
    m_VnlOptimizer->minimize_using_gradient( parameters );
    }
  else
    {
    m_VnlOptimizer->minimize_without_gradient( parameters );
    }

  // InternalParametersType is different than ParametersType....
  ParametersType p(parameters.size());
  for(unsigned int i=0; i < parameters.size(); ++i)
    {
    p[i] = parameters[i];
    }
  this->SetCurrentPosition( p );
      

}

/** Set the maximum number of iterations */
void 
LevenbergMarquardtOptimizer
::SetNumberOfIterations(unsigned int iterations)
{
  if(m_VnlOptimizer)
    { 
    m_VnlOptimizer->set_max_function_evals(iterations);
    }

  m_NumberOfIterations = iterations;
}





/** Set the maximum number of iterations */
void 
LevenbergMarquardtOptimizer
::SetValueTolerance(double tol)
{
  if(m_VnlOptimizer)
    { 
    m_VnlOptimizer->set_x_tolerance(tol);
    }

  m_ValueTolerance = tol;
}


/** Set Gradient Tolerance */
void 
LevenbergMarquardtOptimizer
::SetGradientTolerance(double tol)
{
 if(m_VnlOptimizer)
    { 
    m_VnlOptimizer->set_g_tolerance(tol);
    }

  m_GradientTolerance = tol;

}


/** Set Epsilon function */
void 
LevenbergMarquardtOptimizer
::SetEpsilonFunction(double epsilon)
{
  if(m_VnlOptimizer)
    { 
    m_VnlOptimizer->set_epsilon_function(epsilon);
    }

  m_EpsilonFunction = epsilon;

}


/**
 * Get the Optimizer
 */
vnl_levenberg_marquardt * 
LevenbergMarquardtOptimizer
::GetOptimizer()
{
  return m_VnlOptimizer;
}




} // end namespace itk

#endif
