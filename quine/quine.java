import org.python.core.*;
import org.python.util.PythonInterpreter;

class Quine {
  static PyString loljava = new PyString("import org.python.core.*;%(lf)cimport org.python.util.PythonInterpreter;%(lf)c%(lf)cclass Quine {%(lf)c  static PyString loljava = new PyString(%(dq)c%(loljava)s%(dq)c);%(lf)c  public static void main(String[] args) {%(lf)c    PythonInterpreter py = new PythonInterpreter();%(lf)c    py.set(%(dq)cloljava%(dq)c, loljava);%(lf)c    py.set(%(dq)cdq%(dq)c, 34); %(lf)c    py.set(%(dq)clf%(dq)c, 10);%(lf)c    py.exec(%(dq)cprint loljava %% locals()%(dq)c);%(lf)c  }%(lf)c}%(lf)c");
  public static void main(String[] args) {
    PythonInterpreter py = new PythonInterpreter();
    py.set("loljava", loljava);
    py.set("dq", 34); 
    py.set("lf", 10);
    py.exec("print loljava % locals()");
  }
}

