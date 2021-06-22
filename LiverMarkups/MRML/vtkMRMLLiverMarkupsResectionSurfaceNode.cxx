/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLResectionSurfaceNode.cxx

  Copyright (c) 2017, The Intervention Centre, Oslo University Hospital

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  =========================================================================*/

// This module includes
#include "vtkMRMLLiverMarkupsResectionSurfaceNode.h"
//#include "vtkMRMLResectionSurfaceDisplayNode.h"
//#include "ResectionPlanningModuleDefaultValues.h"

// MRML includes
#include <vtkMRMLModelNode.h>
#include <vtkMRMLScene.h>
#include "vtkMRMLLiverMarkupsSlicingContourNode.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkCommand.h>

#include <vtkEventBroker.h>

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLLiverMarkupsResectionSurfaceNode);

//------------------------------------------------------------------------------
vtkMRMLLiverMarkupsResectionSurfaceNode::vtkMRMLLiverMarkupsResectionSurfaceNode()
//  :ResectionMargin(DEFAULT_RESECTION_MARGIN)
{

  // Initialization of control points
  double startX = -100;
  double startY = -100;
  double endX = 100;
  double endY = 100;
  double incX = (endX - startX)/4.0;
  double incY = (endY - startY)/4.0;

  //Generate geometry;
  for(int i=0; i<4; ++i)
    {
    for(int j=0; j<4; ++j)
      {
      double point[3] = {startX+i*incX, startY+j*incY, 0.0};
      this->ControlPoints->InsertNextPoint(point);
      }
    }
  this->SetControlPointPositionsWorld(this->ControlPoints);
}

//------------------------------------------------------------------------------
vtkMRMLLiverMarkupsResectionSurfaceNode::~vtkMRMLLiverMarkupsResectionSurfaceNode()
{

}

//------------------------------------------------------------------------------
void vtkMRMLLiverMarkupsResectionSurfaceNode::PrintSelf(ostream &vtkNotUsed(os),
                                            vtkIndent vtkNotUsed(nIndent))
{

}

//------------------------------------------------------------------------------
vtkMRMLResectionSurfaceDisplayNode*
vtkMRMLLiverMarkupsResectionSurfaceNode::GetResectionSurfaceDisplayNode()
{
  vtkMRMLDisplayNode *displayNode = this->GetDisplayNode();
 // if (displayNode && displayNode->IsA("vtkMRMLResectionSurfaceDisplayNode"))
 //   {
 //   return vtkMRMLResectionSurfaceDisplayNode::SafeDownCast(displayNode);
 //   }
  return NULL;
}

//------------------------------------------------------------------------------
void vtkMRMLLiverMarkupsResectionSurfaceNode::AddTargetTumor(vtkMRMLModelNode *tumorNode)
{
  if (!tumorNode)
    {
    return;
    }

  if (this->TargetTumors->IsItemPresent(tumorNode))
    {
    return;
    }

  this->TargetTumors->AddItem(tumorNode);
  this->Modified();
}

//------------------------------------------------------------------------------
void vtkMRMLLiverMarkupsResectionSurfaceNode::RemoveTargetTumor(vtkMRMLModelNode *tumorNode)
{
  if (!tumorNode)
    {
    return;
    }

  if (!this->TargetTumors->IsItemPresent(tumorNode))
    {
    return;
    }

  this->TargetTumors->RemoveItem(tumorNode);
  this->Modified();
}

//------------------------------------------------------------------------------
int vtkMRMLLiverMarkupsResectionSurfaceNode::GetNumberOfTargetTumors() const
{
  return this->TargetTumors->GetNumberOfItems();
}

//------------------------------------------------------------------------------
void vtkMRMLLiverMarkupsResectionSurfaceNode::SetControlPoints(vtkPoints *points)
{
  if (!points)
    {
    vtkErrorMacro("No points provided.");
    return;
    }

  this->ControlPoints->DeepCopy(points);
  this->SetControlPointPositionsWorld(this->ControlPoints);
  this->Modified();
}

//------------------------------------------------------------------------------
void vtkMRMLLiverMarkupsResectionSurfaceNode::SetTargetParenchyma(vtkMRMLModelNode *node)
{
  this->TargetParenchyma = node;
  this->InvokeEvent(vtkCommand::ModifiedEvent);
}

//------------------------------------------------------------------------------
vtkMRMLModelNode* vtkMRMLLiverMarkupsResectionSurfaceNode::GetTargetParenchyma() const
{
  return this->TargetParenchyma;
}

//---------------------------------------------------------------------------
void vtkMRMLLiverMarkupsResectionSurfaceNode::ProcessMRMLEvents(vtkObject* caller,
    unsigned long event,
    void* callData)
{
    if (!caller)
    {
        std::cout << "No caller?" << std::endl;
        return;
    }

    //std::cout << "ProcessMRMLEvents: " << caller->GetClassName() << " " << event << std::endl;

    vtkEventBroker* broker = vtkEventBroker::GetInstance();
    if (!broker)
        return;

    if (!broker->GetObservationExist(this->GetScene(), vtkMRMLScene::NodeAddedEvent, this, this->MRMLCallbackCommand))
        broker->AddObservation(this->GetScene(), vtkMRMLScene::NodeAddedEvent, this, this->MRMLCallbackCommand);

    if (event == vtkMRMLScene::NodeAddedEvent)
    {
        vtkMRMLLiverMarkupsSlicingContourNode* node = reinterpret_cast<vtkMRMLLiverMarkupsSlicingContourNode*>(callData);
        if (node)
        {
            //std::cout << "ProcessMRMLEvents got vtkMRMLLiverMarkupsSlicingContourNode" << std::endl;
            if (!broker->GetObservationExist(node, vtkMRMLLiverMarkupsSlicingContourNode::DisplayModifiedEvent, this, this->MRMLCallbackCommand))
                broker->AddObservation(node, vtkMRMLLiverMarkupsSlicingContourNode::DisplayModifiedEvent, this, this->MRMLCallbackCommand);
        }
    }
    else if (event == vtkMRMLLiverMarkupsSlicingContourNode::DisplayModifiedEvent)
    {
        vtkMRMLLiverMarkupsSlicingContourNode* node = reinterpret_cast<vtkMRMLLiverMarkupsSlicingContourNode*>(caller);
        if (node)
        {
            vtkPoints* points = node->GetCurvePoints();
            if(points)
                this->SetControlPoints(points);
        }
    }


    Superclass::ProcessMRMLEvents(caller, event, callData);
}