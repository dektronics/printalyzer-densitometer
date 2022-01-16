from kikit.units import mm
from kikit.common import resolveAnchor
from kikit.defs import EDA_TEXT_HJUSTIFY_T, EDA_TEXT_VJUSTIFY_T, Layer
import pcbnew

def kikitPostprocess(panel, arg):
    # Copy title block from original board
    b = pcbnew.LoadBoard(arg)
    panel.board.SetTitleBlock(b.GetTitleBlock())

    # Add revision information to bottom rail
    position = resolveAnchor("mb")(panel.boardSubstrate.boundingBox())
    position.y -= int(2.5 * mm)
    panel.addText("Rev: ${REVISION} [${ISSUE_DATE}]", position, hJustify=EDA_TEXT_HJUSTIFY_T.GR_TEXT_HJUSTIFY_CENTER)

    # Remove all dimension marks from the board
    for element in panel.board.GetDrawings():
        if isinstance(element, pcbnew.PCB_DIMENSION_BASE) and element.GetLayerName() == "User.Comments":
            panel.board.Remove(element)

    # Add top panel dimension
    dim = pcbnew.PCB_DIM_ALIGNED(panel.board)
    dim.SetStart(resolveAnchor("tl")(panel.boardSubstrate.boundingBox()))
    dim.SetEnd(resolveAnchor("tr")(panel.boardSubstrate.boundingBox()))
    dim.SetHeight(int(-2.5 * mm));
    dim.SetUnits(pcbnew.EDA_UNITS_MILLIMETRES)
    dim.SetLayer(Layer.Cmts_User)
    panel.board.Add(dim)
    
    # Add side panel dimension
    dim = pcbnew.PCB_DIM_ALIGNED(panel.board)
    dim.SetStart(resolveAnchor("tl")(panel.boardSubstrate.boundingBox()))
    dim.SetEnd(resolveAnchor("bl")(panel.boardSubstrate.boundingBox()))
    dim.SetHeight(int(2.5 * mm));
    dim.SetUnits(pcbnew.EDA_UNITS_MILLIMETRES)
    dim.SetLayer(Layer.Cmts_User)
    panel.board.Add(dim)
