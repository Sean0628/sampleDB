namespace indexing {
  class BTreeIndex : public Index {
    public:
      BTreeIndex(tx::Transaction& tx, const std::string& indexName, const record::Layout& leafLayout);
      void beforeFirst(const scan::Constant& key) override;
      bool next() override;
      record::RID getDataRid() const override;
      void insert(const scan::Constant& dataval, const record::RID& datarid) override;
      void remove(const scan::Constant& dataval, const record::RID& datarid) override;
      void close() override;
      static int searchCost(int numBlocks, int rpb);
    private:
      tx::Transaction& _tx;
      record::Layout _dirLayout, _leafLayout;
      std::string _leafTable;
      BTreeLeaf _leaf;
      file::BlockId _rootBlk;
  };
}
