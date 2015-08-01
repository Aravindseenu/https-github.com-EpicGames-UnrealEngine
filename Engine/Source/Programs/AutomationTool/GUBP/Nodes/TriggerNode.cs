﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;

namespace AutomationTool
{
	public class TriggerNode : LegacyBuildNode
	{
        public bool IsTriggered;
		public bool RequiresRecursiveWorkflow;
		public string StateName;
		public string DescriptionText;
		public string ActionText;

		public TriggerNode(GUBP bp, GUBP.WaitForUserInput InNode) : base(bp, InNode)
		{
			AddSubmittersToFailureEmails = false;

			StateName = InNode.GetTriggerStateName();
			DescriptionText = InNode.GetTriggerDescText();
			ActionText = InNode.GetTriggerActionText();
			RequiresRecursiveWorkflow = InNode.TriggerRequiresRecursiveWorkflow();
		}

		public void Activate()
		{ 
			IsTriggered = true;
		}

		public override bool IsSticky
		{
			get { return IsTriggered; }
		}
	}
}
